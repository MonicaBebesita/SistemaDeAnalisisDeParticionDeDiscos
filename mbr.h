/**
 * @file mbr.h
 * @brief Definiciones para discos inicializados con esquema MBR (Master Boot Record).
 * 
 * Este archivo contiene las constantes, estructuras y prototipos de funciones 
 * necesarias para manejar discos con el esquema de partición MBR. Incluye 
 * descriptores de particiones, la definición del MBR completo y funciones 
 * para verificar y describir particiones.
 */
#ifndef MBR_H
#define MBR_H

/** 
 * @def MBR_SIGNATURE
 * @brief Firma del sector de arranque MBR.
 * 
 * Para verificar si un MBR es válido, esta firma debe estar presente 
 * en los últimos dos bytes del sector de arranque (posiciones 510 y 511).
 */
#define MBR_SIGNATURE 0xAA55

/** 
 * @def MBR_TYPE_GPT
 * @brief Tipo de partición GPT en el MBR.
 * 
 * Este valor indica que el sector MBR contiene una partición GPT protectora.
 */
#define MBR_TYPE_GPT 0xEE

/** 
 * @def MBR_TYPE_UNUSED
 * @brief Identificador de partición no utilizada.
 * 
 * Este valor indica que una entrada en la tabla de particiones está vacía 
 * (sin partición asignada).
 */
#define MBR_TYPE_UNUSED 0x00

/** 
 * @def TYPE_NAME_LEN
 * @brief Longitud máxima para la descripción textual de un tipo de partición.
 */
#define TYPE_NAME_LEN 256


/**
 * @struct mbr_partition_descriptor
 * @brief Descriptor de partición MBR.
 * 
 * Esta estructura define los atributos de cada entrada en la tabla de particiones MBR.
 * Cada entrada ocupa exactamente 16 bytes.
 * 
 * @var mbr_partition_descriptor::boot_flag
 * Indica si la partición es arrancable. El valor 0x80 indica una partición activa.
 * 
 * @var mbr_partition_descriptor::reserved
 * Espacio reservado para los demás campos del descriptor de partición.
 */
typedef struct {
	/* La suma completa de bytes de esta estructura debe ser 16 */
 unsigned char boot_flag;        // 1 byte: Indicador de arranque (0x80 o 0x00).
    unsigned char chs_start[3];     // 3 bytes: Dirección CHS de inicio.
    unsigned char partition_type;   // 1 byte: Tipo de partición (e.g., 0x07 para NTFS).
    unsigned char chs_end[3];       // 3 bytes: Dirección CHS de fin.
    unsigned int start_lba;         // 4 bytes: Dirección LBA del sector de inicio.
    unsigned int size;    		
}__attribute__((packed)) mbr_partition_descriptor;


/**
 * @struct mbr
 * @brief Estructura del Master Boot Record.
 * 
 * Representa un sector de arranque completo, que incluye el código de arranque, 
 * la tabla de particiones y la firma del sector.
 * 
 * @var mbr::bootsector_code
 * Código ejecutable del sector de arranque (generalmente 446 bytes).
 * 
 * @var mbr::partition_table
 * Tabla de particiones, que contiene cuatro descriptores de particiones.
 * 
 * @var mbr::signature
 * Firma del sector MBR, que debe ser igual a `0xAA55`.
 */
typedef struct {
	/* TODO definir los atributos para la estructura de datos que representa un MBR (bootsector)*/
	/* La suma completa de bytes de esta estructura debe ser 512 */
	unsigned char bootsector_code[446];
	mbr_partition_descriptor partition_table[4];//4 sectores 16 bytes= 64 bytes
	unsigned short signature; //2 bytes
}__attribute__((packed)) mbr;




/**
 * @brief Imprime la tabla de particiones de un MBR.
 * 
 * Esta función asume que el MBR ya ha sido validado como un MBR tradicional.
 * Recorre cada entrada en la tabla de particiones, imprimiendo sus detalles.
 * 
 * @param boot_record Puntero a la estructura MBR que contiene la tabla de particiones.
 */
void print_mbr_partition_table(mbr *boot_record);






/**
 * @brief Verifica si el MBR es válido y si contiene una partición protectora GPT.
 * 
 * Comprueba si el sector de arranque cumple con los requisitos de un MBR válido:
 * 1. La firma del sector debe ser `0xAA55`.
 * 2. Si es un MBR protector GPT, debe contener al menos una partición con tipo `0xEE`.
 * 
 * @param boot_record Puntero a la estructura `mbr` que contiene el sector de arranque.
 * @return 
 *         - 1 si el MBR es un MBR válido tradicional.
 *         - 2 si el MBR es un MBR protector de GPT.
 *         - 0 si no es un MBR válido.
 */
int is_mbr(mbr * boot_record);


/**
 * @brief Obtiene el nombre textual del tipo de partición.
 * 
 * Esta función toma un tipo de partición (como valor hexadecimal) y llena un buffer 
 * con su descripción textual. Si el tipo no está definido, se retorna "Unknown".
 * 
 * @param type El valor hexadecimal del tipo de partición.
 * @param buf Buffer donde se almacenará el nombre del tipo de partición.
 *            Debe tener capacidad para al menos `TYPE_NAME_LEN` caracteres.
 */
void mbr_partition_type(unsigned char type, char buf[TYPE_NAME_LEN]);


#endif
