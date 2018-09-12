/*
Basic Dreamcast PowerVR(tm) example
(c)2000 Dan Potter

Dreamcast is a registered trademark of Sega Enterprises, Ltd.
PowerVR is a registered trademark of VideoLogic, LLC.


Well, here's the cleaned up source code I promised. It's still not quite
as clean as I'd like ("magic values" =) but it's getting there quite
quickly. Quite a bit of the commentary on what registers do what came
straight from Maiwe's previously released ta-intro.txt which I reference
in here.

This example is not at all complete, quite the opposite. It's more in
the way of a proof of concept. This is pretty much the minimum you can
do and still get polygons on the screen using the PVR. 3D with perspective
transformations in the hardware doesn't work (if it works at all ;-),
there is no texture support, and for all I know changing the number of
polygons for each frame might make it stop working. The background
plane support is very primitive. But hey, it's a start. ^_^

The example uses almost no relevant libdream functions, so there are not
any dependencies on it. I imported the waitvbl routine and the uint types.
The video mode setting routine is considerably more complex than the one
in libdream, as it sets a lot of 3D parameters in the process. This needs
to be cleaned up and integrated into libdream.

The basic rundown of setting up and using 3D support goes something like
this:

1. Set up various 3D and 2D regs to set the mode
2. Create a Tile Accelerator (TA) array structure to direct the TA
   on using allocated VRAM for its scratch space. Each tile on the screen
   (32x32 pixels) needs a buffer.
3. For each frame, point the TA's output at the relevant buffer and
   feed it polygon and vertex data via the store queues to 0x10000000.
   End each display list with a display-list-end item.
4. Wait for the TA to process this data.
5. Add the background plane data in internalized format to the end of
   the TA's scratch space of display list data.
6. Wait for a real full vblank (not interlace fields) and switch display
   start address to to the *other* buffer (note that the TA has in theory
   been rendering to this buffer while you've been feeding it polygon
   data for the next frame). Once the switch is done, start rendering
   for the frame you just fed it.
7. Jump to 3; repeat ad nauseum.

That sounds a bit complex but after looking through the code below it
ought to make more sense. To compile the example:

sh-elf-gcc -m4-single-only -ml -Wl,-Ttext,0x8c010000 -o 3dtest.elf 3dtest.c

This assumes that you have a copy of my libc with crt0 pointing to dc_main
installed in your gcc tree.

The resulting executable (once converted to bin format) is only 3k! I'm
quite pleased with this trend of having very small amounts of code doing
cool things. =) The other difference between this version and the bin
I released previously is that this one does blue and red in the background
(ooooo... ahhhhh! =) and it defaults to only running for a few seconds
for playing around.

Eventually this will be part of libdream... so for the moment, assume that
it's under the MIT/X11 license like normal. Enjoy!

					Dan Potter
					Oct 4, 2000
					http://www.allusion.net/dcdev/
*/


/* Some basic types */
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;

/* Traditional wait-vbl routine; we only use this during
   setup to smooth the screen mode setting; in the rendering
   process you use the 0xa05f6900 register instead, as it
   properly detects interlaced picture completion. */
void vid_waitvbl() {
	volatile uint32 *vbl = (uint32*)0xa05f810c;
	while (!(*vbl & 0x01ff))
		;
	while (*vbl & 0x01ff)
		;
}

/* These values are used during TA render completion; the values
   are swapped out to perform page flipping. */
struct pv_str {
	uint32	view_address;	/* Display start address */
	uint32	ta_struct;	/* TA structure address */
	uint32	ta_scratch;	/* TA scratch space address */
	uint32	output_address;	/* Render output address */
	uint32	zclip;		/* Min Z clip coord (IEEE float) */
	uint32	pclip_x;	/* Pixel Clipping X (IEEE float) */
	uint32	pclip_y;	/* Pixel Clippnig Y (IEEE float) */
	uint32	disp_align;	/* Display align (xwidth*bpp/8) */
	uint32	alpha_mode;	/* Alpha pixel mode */

	/* Two of these aren't precisely known, but they are all related
	   to the TA buffer structure. */
	uint32	next_ta_buf;	/* Next frame's TA buffer space */
	uint32	unknown2;	/* Next frame's ? */
	uint32	next_ta_scratch;/* Next frame's TA scratch space address */
	uint32	unknown3;	/* Next frame's ? */
} page_values[2];

/* Background plane data: this is a sort of truncated normal
   polygon data set. I'm assuming it's stored in internalized
   PVR format. It's a three-vertex polygon strip. */
typedef struct {
	uint32		flags1, flags2;
	uint32		dummy;
	float		x1, y1, z1;
	uint32		argb1;
	float		x2, y2, z2;
	uint32		argb2;
	float		x3, y3, z3;
	uint32		argb3;
} bkg_poly;

/* Global background data structure; this is used during the
   rendering process. */
bkg_poly bkg;


/* Polygon header; each polygon you send to the TA needs to have
   this as its first structure. For flags info, see maiwe's doc. */
typedef struct {
	uint32	flags1, flags2, flags3, flags4;
	uint32	dummy1, dummy2, dummy3, dummy4;
} poly_hdr_t;

/* Vertex structure; each polygon has three or more of these
   arranged in a structure called a strip. If bit 28 is set
   in the flags word, it signifies the end of a strip. Note that
   technically multiple strips don't have to be adjacent but
   they are still part of the same polygon. The example code
   below actually draws _one_ polygon with 12 strips. */
typedef struct {
	uint32	flags;
	float	x, y, z;
	float	a, r, g, b;
} vertex_t;

/* Current page */
int curpage = 0;

/* 3d-specific parameters; these are all about rendering and nothing
   to do with setting up the video, although these do currently assume
   a 640x480x16bit screen. Some stuff in here is still unknown. */
uint32 three_d_parameters[] = {
	0x80a8, 0x15d1c951,	/* M (Unknown magic value) */
	0x80a0, 0x00000020,	/* M */
	0x8008, 0x00000000,	/* TA out of reset */
	0x8048, 0x00000009,	/* alpha config */
	0x8068, 0x02800000,	/* pixel clipping x */
	0x806c, 0x01e00000,	/* pixel clipping y */
	0x8110, 0x00093f39,	/* M */
	0x8098, 0x00800408,	/* M */
	0x804c, 0x000000a0,	/* display align (640*2)/8 */
	0x8078, 0x3f800000,	/* polygon culling (1.0f) */
	0x8084, 0x00000000,	/* M */
	0x8030, 0x00000101,	/* M */
	0x80b0, 0x007f7f7f,	/* Fog table color */
	0x80b4, 0x007f7f7f,	/* Fog vertex color */
	0x80c0, 0x00000000,	/* color clamp min */
	0x80bc, 0xffffffff,	/* color clamp max */
	0x8080, 0x00000007,	/* M */
	0x8074, 0x00000001,	/* cheap shadow */
	0x807c, 0x0027df77,	/* M */
	0x8008, 0x00000001,	/* TA reset */
	0x8008, 0x00000000,	/* TA out of reset */
	0x80e4, 0x00000000,	/* stride width */
	0x6884, 0x00000000,	/* Disable all interrupt events */
	0x6930, 0x00000000,
	0x6938, 0x00000000,
	0x6900, 0xffffffff,	/* Clear all pending int events */
	0x6908, 0xffffffff,
	0x6930, 0x002807ec,	/* Re-enable some events */
	0x6938, 0x0000000e,
	0x80b8, 0x0000ff07,	/* fog density */
	0x80b4, 0x007f7f7f,	/* fog vertex color */
	0x80b0, 0x007f7f7f	/* fog table color */
};

/* We wait for vertical blank (to make it nicer looking) and then
   set these screen parameters. These are equivalent to libdream's
   dc_vid_setup() plus a few extras. */
uint32 scrn_parameters[] = {
	0x80e8, 0x00160008,	/* screen control */
	0x8044, 0x00000000,	/* pixel mode (vb+0x11) */
	0x805c, 0x00000000,	/* Size modulo and display lines (vb+0x17) */
	0x80d0, 0x00000150,	/* interlace flags */
	0x80d8, 0x020c0359,	/* M */
	0x80cc, 0x00150104,	/* M */
	0x80d4, 0x007e0345,	/* horizontal border */
	0x80dc, 0x00240204,	/* vertical position */
	0x80e0, 0x07d6c63f,	/* sync control */
	0x80ec, 0x000000a4,	/* horizontal position */
	0x80f0, 0x00120012,	/* vertical border */
	0x80c8, 0x03450000,	/* set to same as border H in 80d4 */
	0x8068, 0x027f0000,	/* (X resolution - 1) << 16 */
	0x806c, 0x01df0000,	/* (Y resolution - 1) << 16 */
	0x805c, 0x1413b93f,	/* Size modulo and display lines (vb+0x17) */
	0x804c, 0x000000a0,	/* display align */
	0x8118, 0x00008040,	/* M */
	0x80f4, 0x00000401,	/* anti-aliasing */
	0x8048, 0x00000009,	/* alpha config */
	0x8044, 0x00000004,	/* pixel mode (vb+0x11) */
	0x7814, 0x00000000,	/* More interrupt control stuff (so it seems) */
	0x7834, 0x00000000,
	0x7854, 0x00000000,
	0x7874, 0x00000000,
	0x78bc, 0x4659404f,
	0x8040, 0x00000000	/* border color */
};



/* Initialize fog tables; we don't use these right now but
   it's part of the proper setup. */
void ta_fog_init() {
	volatile uint32 *regs = (uint32*)0xa05f0000;
	uint32	idx;
	uint32	value;
	
	for (idx=0x8200, value=0xfffe; idx<0x8400; idx+=4) {
		regs[idx/4] = value;
		value -= 0x101;
	}
}

/* Set up TA buffers. This function takes a base address and sets up
   the TA rendering structures there. Each tile of the screen (32x32) receives
   a small buffer space. This function currently assumes 640x480. */
void ta_create_buffers(uint32 strbase, uint32 bufbase) {
	int x, y;
	volatile uint32 *vr = (uint32*)0xa5000000;
	
	for (x=0; x<0x48; x+=4)
		vr[(strbase+x)/4] = 0;
	vr[(strbase+0x48)/4] = 0x10000000;
	vr[(strbase+0x4c)/4] = 0x80000000;
	vr[(strbase+0x50)/4] = 0x80000000;
	vr[(strbase+0x54)/4] = 0x80000000;
	vr[(strbase+0x58)/4] = 0x80000000;
	vr[(strbase+0x5c)/4] = 0x80000000;
	vr += (strbase+0x60)/4;
	for (x=0; x<(640/32); x++) {
		for (y=0; y<(480/32); y++) {
			/* Note: end-of-list on the last tile! */
			if (x == (640/32)-1 && y == (480/32)-1)
				vr[0] = 0x80000000 | (y << 8) | (x << 2);
			else
				vr[0] = (y << 8) | (x << 2);
			vr[1] = bufbase + 0x500 * y + 0x40 * x;
			vr[2] = strbase | 0x80000000;
			vr[3] = strbase | 0x80000000;
			vr[4] = strbase | 0x80000000;
			vr[5] = strbase | 0x80000000;
			vr += 6;
		}
	}
}

/* Take a series of register/value pairs and set the values */
void set_regs(uint32 *values, uint32 cnt) {
	volatile uint32 long *regs = (uint32*)0xa05f0000;
	int i;
	uint32 r, v;

	for (i=0; i<cnt; i+=2) {
		r = values[i];
		v = values[i+1];
		regs[r/4] = v;
	}
}

/* Set a TA structure buffer; after this completes, all rendering
   and setup operations will reference the given buffer. */
void ta_select_buffer(int which) {
	volatile uint32	*regs = (uint32*)0xa05f0000;
	struct pv_str	*pv = page_values+which;

	regs[0x8008/4] = 1;		/* Reset TA */
	regs[0x8008/4] = 0;
	regs[0x8124/4] = pv->next_ta_buf;
	regs[0x812c/4] = pv->unknown2;
	regs[0x8128/4] = pv->next_ta_scratch;
	regs[0x8130/4] = pv->unknown3;
	regs[0x813c/4] = 0x000e0013;	/* Tile count: (480/32-1) << 16 | (640/32-1) */
	regs[0x8164/4] = pv->next_ta_buf;
	regs[0x8140/4] = 0x00100002;
	regs[0x8144/4] = 0x80000000;	/* Confirm settings */
	(void)regs[0x8144/4];
}

/* Set display start address: assumes 640x480x16bit interlaced */
void vid_set_start(uint32 start) {
	volatile uint32 *regs = (uint32*)0xa05f0000;
	
	regs[0x8050/4] = start;
	regs[0x8054/4] = start + 640*2;
}

/* Clear all of PVR memory */
void vid_full_clear() {
	int i;
	volatile uint32 *vram_l = (uint32*)0xa5000000;
	for (i=0; i<(8*1024*1024) / 4; i++)
		vram_l[i] = 0;
}


/* Prepare the TA for page flipped 3D */
void ta_setup() {
	int i;
	volatile unsigned long *regs = (unsigned long*)0xa05f0000;

	/* Prepare TA value structure */
#define pv page_values[0]
	pv.view_address	= 0x00600000;
	pv.ta_struct	= 0x005683c8;
	pv.ta_scratch	= 0x00400000;
	pv.output_address = 0x00200000;
	pv.zclip	= 0x3e4cccc0;		/* 0.2f */
	pv.pclip_x	= 0x027f0000;
	pv.pclip_y	= 0x01df0000;
	pv.disp_align	= 0x000000a0;		/* (640*2)/8 */
	pv.alpha_mode	= 0x00000009;
	pv.next_ta_buf	= 0x00163880;
	pv.unknown2	= 0x0010e800;
	pv.next_ta_scratch = 0x00000000;
	pv.unknown3	= 0x0010e740;
#undef pv
#define pv page_values[1]
	pv.view_address	= 0x00200000;
	pv.ta_struct	= 0x001683c8;
	pv.ta_scratch	= 0x00000000;
	pv.output_address = 0x00600000;
	pv.zclip	= 0x3e4cccc0;		/* 0.2f */
	pv.pclip_x	= 0x027f0000;
	pv.pclip_y	= 0x01df0000;
	pv.disp_align	= 0x000000a0;		/* (640*2)/8 */
	pv.alpha_mode	= 0x00000009;
	pv.next_ta_buf	= 0x00563880;
	pv.unknown2	= 0x0050e800;
	pv.next_ta_scratch = 0x00400000;
	pv.unknown3	= 0x0050e740;
#undef pv

	/* Setup basic 3D parameters */
	set_regs(three_d_parameters, sizeof(three_d_parameters)/4);
	ta_fog_init();

	/* Set screen mode parameters */
	vid_waitvbl();
	set_regs(scrn_parameters, sizeof(scrn_parameters)/4);	

	/* Point at the second set of buffer structures, 
	   and build said structures. */
	ta_select_buffer(1);
	ta_create_buffers(0x568380, 0x563880);

	/* Now setup the first frame */
	ta_select_buffer(0);
	ta_create_buffers(0x168380, 0x163880);

	/* Set starting render output addresses */
	regs[0x8060/4] = 0x00200000;	/* render output address */
	regs[0x8064/4] = 0x00600000;	/* render to texture output address */
	
	/* Set display start address */
	vid_set_start(0x00200000);

	/* Point back at the second output buffer */
	ta_select_buffer(1);

	/* Unblank screen and set display enable */
	regs[0x80e8/4] &= ~0x00000008;	/* Unblank */
	regs[0x8044/4] |= 0x00000001;	/* Display enable */

	/* Set current page */
	curpage = 0;
}


/* Copy data 4 bytes at a time */
void copy4(uint32 *dest, uint32 *src, int bytes) {
	bytes = bytes / 4;
	while (bytes-- > 0) {
		*dest++ = *src++;
	}
}

/* Send a store queue full of data to the TA */
void ta_send_queue(void *sql, int size) {
	volatile unsigned long *regs = (unsigned long*)0xff000038;

	/* Set store queue destination == tile accelerator */
	regs[0] = regs[1] = 0x10;

	/* Post the first queue */
	copy4((uint32*)0xe0000000, (uint32*)sql, size);
	asm("mov	#0xe0,r0");
	asm("shll16	r0");
	asm("shll8	r0");
	asm("pref	@r0");

	/* If there was a second queue... */
	if (size == 64) {
		asm("mov	#0xe0,r0");
		asm("shll16	r0");
		asm("shll8	r0");
		asm("or		#0x20,r0");
		asm("pref	@r0");
	}
}

/* Begin the rendering process for one frame */
void ta_begin_render() {
	/* Clear all pending events */
	volatile unsigned long *pvrevt = (unsigned long*)0xa05f6900;
	*pvrevt = 0xffffffff;
}

/* Commit a polygon header to the TA */
void ta_commit_poly_hdr(void *polyhdr) {
	ta_send_queue(polyhdr, 32);
}

/* Commit a vertex to the TA */
void ta_commit_vertex(void *vertex) {
	ta_send_queue(vertex, 32);
}

/* Commit an end-of-list to the TA */
void ta_commit_eol() {
	uint32	words[8] = { 0 };
	ta_send_queue(words, 32);
}

/* Finish rendering a frame; this assumes you have written
   a completed display list to the TA. It sets everything up and
   waits for the next vertical blank period to switch buffers. */
void ta_finish_frame() {
	int i, taend;
	volatile unsigned long *regs = (unsigned long*)0xa05f0000;
	volatile unsigned long *vrl = (unsigned long *)0xa5000000;
	struct pv_str	*pv;
	uint32 *bkgdata = (uint32*)&bkg;

	/* Wait for TA to finish munching */
	while (!(regs[0x6900/4] & 0x80))
		;
	regs[0x6900/4] = 0x80;

	/* Throw the background data on the end of the TA's list */
	taend = regs[0x8138/4];
	for (i=0; i<0x40; i+=4)
		vrl[(i+taend)/4] = bkgdata[i/4];
	vrl[(0x44+taend)/4] = 0;	/* not sure if this is required */
	
	/* Wait for the VB to get here */
	regs[0x6900/4] = 0x08;
	while (!(regs[0x6900/4] & 0x08))
		;
	regs[0x6900/4] = 0x08;

	/* Find the register values for the current page */
	pv = page_values + curpage;

	/* Calculate background value for below */
	/* Small side note: during setup, the value is originally
	   0x01203000... I'm thinking that the upper word signifies
	   the length of the background plane list in dwords
	   shifted up by 4. */
	taend = 0x01000000 | ((taend - pv->ta_scratch) << 1);
	
	/* Switch start address to the "good" buffer */
	vid_set_start(pv->view_address);
		
	/* Finish up rendering the current frame (into the other buffer) */
	regs[0x802c/4] = pv->ta_struct;
	regs[0x8020/4] = pv->ta_scratch;
	regs[0x8060/4] = pv->output_address;
	regs[0x808c/4] = taend;			/* Bkg plane location */
	regs[0x8088/4] = pv->zclip;
	regs[0x8068/4] = pv->pclip_x;
	regs[0x806c/4] = pv->pclip_y;
	regs[0x804c/4] = pv->disp_align;
	regs[0x8048/4] = pv->alpha_mode;
	regs[0x8014/4] = 0xffffffff;		/* Go! */
	
	/* Reset TA, switch buffers */
	ta_select_buffer(curpage);

	/* Swap out pages */
	curpage ^= 1;
}


/* A little test program -- creates twelve rainbow polygons and
   moves them around over a color-shifting background. */
typedef struct {
	float	x, y, z;
	float	dx, dy;
} polyplace_t;

polyplace_t polys[12] = {
	{ 16.0f, 0.0f, 10.0f, 3.0f, 5.0f },	/* Top */
	{ 300.0f, 0.0f, 11.0f, 0.0f, 6.0f },
	{ 624.0f, 0.0f, 12.0f, -3.0f, 7.0f },
	
	{ 24.0f, 300.0f, 13.0f, 3.0f, -7.0f },	/* Bottom */
	{ 340.0f, 300.0f, 14.0f, 0.0f, -6.0f },
	{ 616.0f, 300.0f, 15.0f, -3.0f, -5.0f },
	
	{ 0.0f, 16.0f, 16.0f, 5.0f, 3.0f },	/* Left */
	{ 0.0f, 220.0f, 17.0f, 6.0f, 0.0f },
	{ 0.0f, 364.0f, 18.0f, 7.0f, -3.0f },
	
	{ 480.0f, 24.0f, 19.0f, -7.0f, 3.0f },	/* Right */
	{ 480.0f, 260.0f, 20.0f, -6.0f, 0.0f },
	{ 480.0f, 356.0f, 21.0f, -5.0f, -3.0f },
};

/* Does one frame of polygon movement */
void move_polys() {
	int i;
	polyplace_t *p;
	
	for (i=0; i<12; i++) {
		p = polys+i;
		
		p->x += p->dx;
		if (p->x < 0 || p->x > 580.0f) {
			p->dx = -p->dx;
			p->x += p->dx;
		}

		p->y += p->dy;
		if (p->y < 0 || p->y > 420.0f) {
			p->dy = -p->dy;
			p->y += p->dy;
		}
	}
}

/* Sends one polygon's worth of data to the TA */
void draw_one_poly(polyplace_t *p) {
	vertex_t vert;
	
	vert.flags = 0xe0000000;
	vert.x = p->x + 0.0f;
	vert.y = p->y + 200.0f;
	vert.z = p->z;
	vert.a = 1.0f;
	vert.r = 0.0f;
	vert.g = 0.0f;
	vert.b = 1.0f;
	ta_commit_vertex(&vert);

	vert.flags = 0xe0000000;
	vert.x = p->x + 0.0f;
	vert.y = p->y + 0.0f;
	vert.z = p->z;
	vert.a = 1.0f;
	vert.r = 0.0f;
	vert.g = 1.0f;
	vert.b = 0.0f;
	ta_commit_vertex(&vert);

	vert.flags = 0xe0000000;
	vert.x = p->x + 200.0f;
	vert.y = p->y + 200.0f;
	vert.z = p->z;
	vert.a = 1.0f;
	vert.r = 0.0f;
	vert.g = 0.0f;
	vert.b = 0.0f;
	ta_commit_vertex(&vert);

	vert.flags = 0xe0000000 | 0x10000000;
	vert.x = p->x + 200.0f;
	vert.y = p->y + 0.0f;
	vert.z = p->z;
	vert.a = 1.0f;
	vert.r = 1.0f;
	vert.g = 0.0f;
	vert.b = 0.0f;
	ta_commit_vertex(&vert);
}

uint32 color = 0; int dcolor = 5;
void draw_frame() {
	poly_hdr_t	poly;
	int i;

	/* Setup a background plane */
	bkg.flags1 = 0x90800000;
	bkg.flags2 = 0x20800440;
	bkg.dummy = 0;
	bkg.x1 = 0.0f;
	bkg.y1 = 480.0f;
	bkg.z1 = 0.2f;
	bkg.argb1 = 0xff000000 | ((255 - color) << 16);
	bkg.x2 = 0.0f;
	bkg.y2 = 0.0f;
	bkg.z3 = 0.2f;
	bkg.argb2 = 0xff000000;
	bkg.x3 = 640.0f;
	bkg.y3 = 480.0f;
	bkg.z3 = 0.2f;
	bkg.argb3 = 0xff000000 | color;

	color += dcolor;
	if (color <= 0) {
		color = 0; dcolor = -dcolor;
	}
	if (color >= 255) {
		color = 255; dcolor = -dcolor;
	}

	/* Send polygon header to the TA using store queues */
	poly.flags1 = 0x80870012;
	poly.flags2 = 0x90800000;
	poly.flags3 = 0x20800440;
	poly.flags4 = 0x00000000;
	poly.dummy1 = poly.dummy2 = poly.dummy3 = poly.dummy4 = 0xffffffff;
	ta_commit_poly_hdr(&poly);

	/* Draw all polygons */
	for (i=0; i<12; i++) {
		draw_one_poly(polys+i);
	}

	/* end of list */
	ta_commit_eol();
	
	/* Finish the frame */
	ta_finish_frame();
	
	/* Move all polygons */
	move_polys();
}

/* Main program: init and loop drawing polygons */
int dc_main() {
	int i;
	
	vid_full_clear();
	ta_setup();

	/*while (1)	// enable for endless loop */
	for (i=0; i<240; i++)	/* 4 seconds of frames (NTSC) */
		draw_frame();
		
	return 0;
}

