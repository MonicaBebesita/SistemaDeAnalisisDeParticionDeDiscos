
#include "mbr.h"


//Constante firma para todas las cabeceras de GPT 
#define GPT_HEADER_SIGNATURE 0x5452415020494645
/**
 * @struct guid
 * @brief Representación de un GUID (Globally Unique Identifier).
 * 
 * Un GUID es un identificador único utilizado en particiones GPT y otros contextos.
 * Para más detalles, consulte la especificación UEFI: 
 * https://uefi.org/specs/UEFI/2.10/Apx_A_GUID_and_Time_Formats.html
 * 
 * @var guid::time_lo
 * Campo bajo del timestamp.
 * @var guid::time_mid
 * Campo medio del timestamp.
 * @var guid::time_hi_and_version
 * Campo alto del timestamp y versión.
 * @var guid::clock_seq_hi_and_reserved
 * Campo alto de la secuencia de reloj.
 * @var guid::clock_seq_lo
 * Campo bajo de la secuencia de reloj.
 * @var guid::node
 * Identificador único espacial (6 bytes).
 */
typedef struct {
	unsigned int time_lo;                   ///< Campo bajo del timestamp.
    unsigned short time_mid;                ///< Campo medio del timestamp.
    unsigned short time_hi_and_version;     ///< Campo alto del timestamp y versión.
    unsigned char clock_seq_hi_and_reserved;///< Campo alto de la secuencia de reloj.
    unsigned char clock_seq_lo;             ///< Campo bajo de la secuencia de reloj.
    unsigned char node[6];                  ///< Identificador único espacial.
}__attribute__((packed))guid;


/**
 * @struct gpt_header
 * @brief Encabezado de la tabla GPT.
 * 
 * Representa el encabezado de una tabla de particiones GPT. 
 * Contiene información sobre el esquema y la tabla de particiones.

 */
typedef struct {
	unsigned long long signature; 			 /*!< GPT signature */
	unsigned int revision; 					 /*!< Revision */
	unsigned int header_size; 				 /*!< Header size */
	unsigned int header_crc32; 				 /*!< CRC32 of the header */
	unsigned int reserved; 					 /*!< Reserved */
	unsigned long long my_lba; 				 /*!< LBA of the header */
	unsigned long long alternate_lba; 		 /*!< LBA of the alternate header */
	unsigned long long first_usable_lba;	 /*!< First usable LBA for partitions */
	unsigned long long last_usable_lba; 	 /*!< Last usable LBA for partitions */
	guid disk_guid; 						 /*!< Disk GUID */
	unsigned long long partition_entry_lba;  /*!< LBA of the partition table */
	unsigned int num_partition_entries; 	 /*!< Number of partition entries */
	unsigned int size_partition_entry; 		 /*!< Size of a partition entry */
	unsigned int partition_entry_array_crc32;/*!< CRC32 of the partition entry array */
	unsigned char content[420]; 			 /*!< Reserved */
}__attribute__((packed)) gpt_header;




/**
 * @struct gpt_partition_descriptor
 * @brief Descriptor de partición GPT.
 * 
 * Representa una entrada en la tabla de particiones GPT. 
 * Cada entrada describe una partición en el disco.
 */typedef struct {
    guid partition_type_guid;   // GUID del tipo de partición
    unsigned char unique_partition_guid[16];   // GUID único de la partición
    unsigned long long starting_lba;   // LBA de inicio
    unsigned long long ending_lba;     // LBA de fin
    unsigned long long attributes;    // Atributos
    unsigned char partition_name[72];  // Nombre de la partición
    // Reservado 0 bytes
} __attribute__((packed)) gpt_partition_descriptor;

/**
 * @brief imprime la tabla de particiones del mbr de proteccion
 * @param boot_record mbr de proteccion encontrado
 */
void print_gpt_protective_mbr_table();

/**
 * @brief imprime la tabla de particiones de gpt
 * @param partition variable que describe los elementos importantes del descriptor de particiones de gpt
 */
void print_gpt_partition_table(gpt_partition_descriptor *partition);
/**
 * @brief imprime la cabecera del gpt
 * @param hdr es la cabecera
 */
void print_gpt_header(gpt_header * hdr);
/**
 * @struct gpt_partition_type
 * @brief Tipo de partición GPT.
 * 
 * Contiene información descriptiva sobre un tipo de partición GPT.
 * 
 * @var gpt_partition_type::os
 * Sistema operativo asociado al tipo de partición.
 * @var gpt_partition_type::description
 * Descripción textual del tipo de partición.
 * @var gpt_partition_type::guid
 * GUID asociado al tipo de partición.
 */
typedef struct {
	const char *os;           ///< Sistema operativo asociado.
    const char *description;  ///< Descripción del tipo de partición.
    const char *guid;         ///< GUID del tipo de partición.
} gpt_partition_type;


/**
 * @brief Obtiene la descripción de un tipo de partición GPT.
 * 
 * Devuelve información descriptiva de un tipo de partición GPT basado en su GUID.
 * 
 * @param guid_str GUID de la partición en formato de cadena.
 * @return Puntero a una estructura `gpt_partition_type` con la descripción del tipo de partición.
 */
const gpt_partition_type* get_gpt_partition_type(char * guid_str);

/**
 * @brief Decodifica el nombre de una partición GPT.
 * 
 * Convierte un nombre de partición codificado en dos bytes por carácter 
 * en una cadena legible por humanos.
 * 
 * @param name Nombre de la partición codificado (72 bytes).
 * @return Puntero a una cadena con el nombre decodificado.
 */
char *gpt_decode_partition_name(char name[72]);

/**
 * @brief Verifica si un sector de arranque es un MBR protector.
 * 
 * Un MBR protector indica la presencia de una tabla de particiones GPT.
 * 
 * @param boot_record Sector de arranque leído en memoria.
 * @return 1 si el sector es un MBR protector, 0 en caso contrario.
 */
int is_protective_mbr(mbr *boot_record);

/**
 * @brief Verifica si un encabezado GPT es válido.
 * 
 * Comprueba la validez del encabezado de la tabla GPT según las especificaciones.
 * 
 * @param hdr Puntero al encabezado GPT.
 * @return 1 si el encabezado es válido, 0 en caso contrario.
 */
int is_valid_gpt_header(gpt_header *hdr);

/**
 * @brief Verifica si un descriptor de partición GPT está vacío.
 * 
 * Un descriptor se considera vacío si su GUID de tipo de partición es nulo.
 * 
 * @param desc Descriptor de partición.
 * @return 1 si el descriptor está vacío, 0 en caso contrario.
 */
int is_null_descriptor(gpt_partition_descriptor *desc);

/**
 * @brief Crea una representación legible de un GUID.
 * 
 * Convierte un GUID en su representación textual en formato estándar.
 * 
 * @param buf Buffer que contiene el GUID.
 * @return Puntero a una nueva cadena con la representación textual del GUID.
 */
char *guid_to_str(guid *buf);
