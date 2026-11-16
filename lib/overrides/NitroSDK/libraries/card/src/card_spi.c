/*
 * The game's card_spi.c knows one backup device more than the fetched release: a 128 KB EEPROM (0x020000 bytes), with
 * 256-byte pages and three address bytes, which CARDi_IdentifyBackupCore (0x020105CC) describes between the 64 KB
 * EEPROM and the flash devices. Written from the game's code; see tools/override_functions.py for how this file is
 * used. The game keeps no other function of the file.
 */

/* override: replace CARDi_IdentifyBackupCore */
void CARDi_IdentifyBackupCore(CARDBackupType type)
{
    CARDiCommandArg *const arg = cardi_common.cmd;

    MI_CpuFill8(&arg->spec, 0, sizeof(arg->spec));
    arg->type = type;
    arg->spec.caps = CARD_BACKUP_CAPS_AVAILABLE | CARD_BACKUP_CAPS_READ_STATUS;
    if (type == CARD_BACKUP_TYPE_NOT_USE) {
        return;
    }
    {
        const u32 bytes = (u32)(1 << ((type >> CARD_BACKUP_TYPE_SIZEBIT_SHIFT) & CARD_BACKUP_TYPE_SIZEBIT_MASK));
        const int kind = (type >> CARD_BACKUP_TYPE_DEVICE_SHIFT) & CARD_BACKUP_TYPE_DEVICE_MASK;
        const int maker = (type >> CARD_BACKUP_TYPE_VENDER_SHIFT) & CARD_BACKUP_TYPE_VENDER_MASK;

        arg->spec.total_size = bytes;
        arg->spec.initial_status = 0xFF;

        if (kind == CARD_BACKUP_TYPE_DEVICE_EEPROM) {
            switch (bytes) {
            default:
                goto unsupported;
            case 0x000200:
                arg->spec.page_size = 0x10;
                arg->spec.addr_width = 1;
                arg->spec.program_page = 5;
                arg->spec.initial_status = 0xF0;
                break;
            case 0x002000:
                arg->spec.page_size = 0x20;
                arg->spec.addr_width = 2;
                arg->spec.program_page = 5;
                arg->spec.initial_status = 0x00;
                break;
            case 0x010000:
                arg->spec.page_size = 0x80;
                arg->spec.addr_width = 2;
                arg->spec.program_page = 10;
                arg->spec.initial_status = 0x00;
                break;
            case 0x020000:
                arg->spec.page_size = 0x100;
                arg->spec.addr_width = 3;
                arg->spec.program_page = 5;
                arg->spec.initial_status = 0x00;
                break;
            }
            arg->spec.sect_size = arg->spec.page_size;
            arg->spec.caps |= CARD_BACKUP_CAPS_READ;
            arg->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
            arg->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
            arg->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
        } else if (kind == CARD_BACKUP_TYPE_DEVICE_FLASH) {
            switch (bytes) {
            default:
                goto unsupported;
            case 0x040000:
            case 0x080000:
            case 0x100000:
                arg->spec.write_page = 25;
                arg->spec.write_page_total = 300;
                arg->spec.erase_page = 300;
                arg->spec.erase_sector = 5000;
                arg->spec.caps |= CARD_BACKUP_CAPS_WRITE;
                arg->spec.caps |= CARD_BACKUP_CAPS_ERASE_PAGE;
                break;
            case 0x200000:
                arg->spec.write_page = 23;
                arg->spec.write_page_total = 300;
                arg->spec.erase_sector = 500;
                arg->spec.erase_sector_total = 5000;
                arg->spec.erase_chip = 10000;
                arg->spec.erase_chip_total = 60000;
                arg->spec.initial_status = 0x00;
                arg->spec.caps |= CARD_BACKUP_CAPS_WRITE;
                arg->spec.caps |= CARD_BACKUP_CAPS_ERASE_PAGE;
                arg->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
                arg->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
                break;
            case 0x400000:
                arg->spec.erase_sector = 600;
                arg->spec.erase_sector_total = 3000;
                arg->spec.erase_subsector = 70;
                arg->spec.erase_subsector_total = 150;
                arg->spec.erase_chip = 23000;
                arg->spec.erase_chip_total = 800000;
                arg->spec.initial_status = 0x00;
                arg->spec.subsect_size = 0x1000;
                arg->spec.caps |= CARD_BACKUP_CAPS_ERASE_SUBSECTOR;
                arg->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
                arg->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
                break;
            case 0x800000:
                if (maker == 0) {
                    arg->spec.erase_sector = 1000;
                    arg->spec.erase_sector_total = 3000;
                    arg->spec.erase_chip = 68000;
                    arg->spec.erase_chip_total = 160000;
                    arg->spec.initial_status = 0x00;
                    arg->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
                    arg->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
                } else if (maker == 1) {
                    arg->spec.erase_sector = 1000;
                    arg->spec.erase_sector_total = 3000;
                    arg->spec.erase_chip = 68000;
                    arg->spec.erase_chip_total = 160000;
                    arg->spec.initial_status = 0x84;
                    arg->spec.caps |= CARD_BACKUP_CAPS_ERASE_CHIP;
                    arg->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
                }
                break;
            }
            arg->spec.sect_size = 0x10000;
            arg->spec.page_size = 0x100;
            arg->spec.addr_width = 3;
            arg->spec.program_page = 5;
            arg->spec.caps |= CARD_BACKUP_CAPS_READ;
            arg->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
            arg->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
            arg->spec.caps |= CARD_BACKUP_CAPS_ERASE_SECTOR;
        } else if (kind == CARD_BACKUP_TYPE_DEVICE_FRAM) {
            if (bytes != 0x002000 && bytes != 0x008000) {
                goto unsupported;
            }
            arg->spec.page_size = bytes;
            arg->spec.sect_size = bytes;
            arg->spec.addr_width = 2;
            arg->spec.initial_status = 0x00;
            arg->spec.caps |= CARD_BACKUP_CAPS_READ;
            arg->spec.caps |= CARD_BACKUP_CAPS_PROGRAM;
            arg->spec.caps |= CARD_BACKUP_CAPS_VERIFY;
            arg->spec.caps |= CARD_BACKUP_CAPS_WRITE_STATUS;
        } else {
            goto unsupported;
        }
    }
    return;
unsupported:
    // One assignment gives the original's order of the stores and the load of cardi_common
    arg->spec.total_size = (u32)(arg->type = CARD_BACKUP_TYPE_NOT_USE);
    cardi_common.cmd->result = CARD_RESULT_UNSUPPORTED;
}
