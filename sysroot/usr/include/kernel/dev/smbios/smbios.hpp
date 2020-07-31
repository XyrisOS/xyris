/**
 * @file smbios.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.3
 * @date 2019-11-15
 *
 * @copyright Copyright the Panix Contributors (c) 2019
 *
 */

#ifndef PANIX_SMBIOS_HPP
#define PANIX_SMBIOS_HPP

#include <stdint.h>
#include <stddef.h>

// TODO: Need to make a function that will read the info from the SMBIOS into this struct
struct SMBIOSEntryPoint {
 	int8_t EntryPointString[4];     //This is _SM_
 	uint8_t Checksum;               //This value summed with all the values of the table, should be 0 (overflow)
 	uint8_t Length;                 //Length of the Entry Point Table. Since version 2.1 of SMBIOS, this is 0x1F
 	uint8_t MajorVersion;           //Major Version of SMBIOS
 	uint8_t MinorVersion;           //Minor Version of SMBIOS
 	uint16_t MaxStructureSize;      //Maximum size of a SMBIOS Structure (we will se later)
 	uint8_t EntryPointRevision;     //...
 	int8_t FormattedArea[5];        //...
 	int8_t EntryPointString2[5];    //This is _DMI_
 	uint8_t Checksum2;              //Checksum for values from EntryPointString2 to the end of table
 	uint16_t TableLength;           //Length of the Table containing all the structures
 	uint32_t TableAddress;          //Address of the Table
 	uint16_t NumberOfStructures;    //Number of structures in the table
 	uint8_t BCDRevision;            //Unused
 };

 struct SMBIOSHeader {
 	uint8_t type;
 	uint8_t len;
 	uint16_t handle;
 };

char* px_get_smbios_addr();
size_t px_get_smbios_table_len(struct SMBIOSHeader *hd);

#endif /* PANIX_SMBIOS_HPP */