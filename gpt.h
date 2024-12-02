/**
 * @file gpt.h
 * @author 
 * Erwin Meza Vega <emezav@unicauca.edu.co>
 * @brief Definiciones para discos inicializados con esquema GPT (GUID Partition Table).
 * 
 * Este archivo contiene las definiciones de estructuras, constantes y funciones necesarias
 * para manejar discos que utilizan el esquema de partición GPT. Incluye representaciones
 * de GUIDs, encabezados GPT y descriptores de particiones.
 * 
 * @copyright MIT License
 */

#ifndef GPT_H
#define GPT_H

#include "mbr.h"
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
	/* TODO definir los atributos del encabezado de la tabla GPT */
}__attribute__((packed)) gpt_header;



/**
 * @struct gpt_partition_descriptor
 * @brief Descriptor de partición GPT.
 * 
 * Representa una entrada en la tabla de particiones GPT. 
 * Cada entrada describe una partición en el disco.
 */
typedef struct {
	/* TODO definir los atributos de un descriptor de particion GPT */
}__attribute__((packed)) gpt_partition_descriptor;



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

#endif