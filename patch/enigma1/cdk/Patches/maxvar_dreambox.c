/*
 * dreambox.c - mapper for DreamBOX board. based on redwood.c.
 *
 *
 * Copyright 2002 Felix Domke <tmbinc@gmx.net>
 *
 * original:
 * Copyright 2001 MontaVista Softare Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR   IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT,  INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  History: 12/17/2001 - Armin
 *  		migrated to use do_map_probe
 *
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>

#define WINDOW_ADDR 0x7f800000
#define WINDOW_SIZE 0x00800000

#define VPP_SWITCHING

volatile u32 *gpio=0;

void dreambox_flash_set_vpp( struct map_info * map, int i)
{
#ifdef VPP_SWITCHING
    gpio[1] |= (1 << 19);
    gpio[2] &= ~(0x3 << 6);
    gpio[4] &= ~(0x3 << 6);
    gpio[6] &= ~(1 << 19);

    if (i)
	gpio[0] |= (1 << 19);
    else
	gpio[0] &= ~(1 << 19);
#endif
}

struct map_info dreambox_flash_map = {
	name: "DreamBOX rev3+",
	size: WINDOW_SIZE,
	bankwidth: 2,
	set_vpp: dreambox_flash_set_vpp
};

static struct mtd_partition dreambox_flash_partitions[] = {
	{
		name: "DreamBOX cramfs+squashfs",
		offset: 0x000000,
		size:   0x500000,
	},
	{
		name: "DreamBOX jffs2",
		offset: 0x500000,
		size:   0x2c0000,
	},
	{
		name: "DreamBOX OpenBIOS",
		offset: 0x7c0000,
		size:   0x040000,
		mask_flags: MTD_WRITEABLE       /* force read-only */
	},
	{
		name: "DreamBOX (w/o bootloader)",
		offset: 0,
		size:   0x7c0000,
	},
	{
		name: "DreamBOX (w/ bootloader)",
		offset: 0,
		size:   0x800000,
		mask_flags: MTD_WRITEABLE       /* force read-only */
	},
	{
		name: "DreamBOX SquashedFS",
		offset: 0x120000,
		size:	0x3e0000
	},
	{
		name: "DreamBOX Cramfs",
		offset: 0,
		size:	0x120000
	}
};
#define NUM_DREAMBOX_FLASH_PARTITIONS \
	(sizeof(dreambox_flash_partitions)/sizeof(dreambox_flash_partitions[0]))

static struct mtd_info *dreambox_mtd;

int __init init_dreambox_flash(void)
{
	printk(KERN_NOTICE "dreambox: flash mapping: %x at %x\n",
	       WINDOW_SIZE, WINDOW_ADDR);

	dreambox_flash_map.phys = WINDOW_ADDR;
	dreambox_flash_map.virt =
		(unsigned long)ioremap(WINDOW_ADDR, WINDOW_SIZE);

	if (!dreambox_flash_map.virt) {
		printk("init_dreambox_flash: failed to ioremap\n");
		return -EIO;
	}
	
	simple_map_init(&dreambox_flash_map);

	dreambox_flash_map.map_priv_1 = 0;
	dreambox_flash_map.map_priv_2 = 0;
	dreambox_mtd = do_map_probe("cfi_probe", &dreambox_flash_map);

	if (dreambox_mtd) {
		gpio=ioremap(0x40060000, 4096);
		dreambox_flash_set_vpp(0,0);
		printk("disable flash VPP\n");
		dreambox_mtd->owner = THIS_MODULE;
		return add_mtd_partitions(dreambox_mtd,
					  dreambox_flash_partitions,
					  NUM_DREAMBOX_FLASH_PARTITIONS);
	}

	return -ENXIO;
}

static void __exit cleanup_dreambox_flash(void)
{
	if (dreambox_mtd) {
		iounmap((void*)gpio);
		del_mtd_partitions(dreambox_mtd);
		iounmap((void *)dreambox_flash_map.virt);
		map_destroy(dreambox_mtd);
	}
}

module_init(init_dreambox_flash);
module_exit(cleanup_dreambox_flash);
