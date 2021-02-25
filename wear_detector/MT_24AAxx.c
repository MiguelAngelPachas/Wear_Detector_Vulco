/* Inclusi�n de archivos */
#include <stdint.h>
#include "project_defines.h"
#include <util/delay.h>
#include "MT_twi_master.h"
#include "MT_24AAxx.h"

/* Definici�n de funciones */


/****************************************************************************
* Nombre de la funci�n: EEPROM_Write
* retorna : el estado del bus I2C/TWI (si hubo alg�n error en la transmisi�n)
* arg : slave_addr (direcci�n de esclavo del chip de memoria)
* arg : mem_addr (ubicaci�n de memoria donde se desea empezar a escribir)
* arg : n_bytes (cantidad de bytes que se desean escribir en la memoria)
* arg: buffer (puntero que indica el arreglo donde se almacenan los datos
*       que ser�n enviados)
* Descripci�n : Recibe como par�metro la direcci�n de esclavo del chip de
* 				memoria donde se desea almacenar los datos. Los 'n' bytes que
* 				se desean escribir se encuentran almacenados en el arreglo
* 				indicado por el par�metro buffer. Estos datos se empezar�n a
* 				escribir a partir de la ubicaci�n de memoria mem_addr.
* Notas : En estos dispositivos, la ubicaci�n de memoria (inicializada en
*  		  mem_addr) se incrementa de forma autom�tica cada vez que se escribe
*  		  un dato. SIN EMBARGO, esto solo se cumple en direcciones agrupadas
*  		  en p�ginas de 32 ubicaciones. Por ejemplo: De 0 a 31 (P�gina 0),
*  		  de 32 a 63 (P�gina 1), de 64 a 95 (P�gina 2), etc.
*  		  Si se inicializa la ubicaci�n de memoria con mem_addr = 30 y se
*  		  transmiten 5 bytes, el incremento autom�tico solo funcionar� hasta
*  		  la ubicaci�n 31, por lo cualesta funci�n no trabajar� correctamente.
*****************************************************************************/
uint8_t EEPROM_24AAxx_Write(uint8_t slave_addr, uint16_t mem_addr, uint8_t n_bytes, uint8_t *buffer){

	uint8_t mem_address[2];
	uint8_t salida = I2C_OK;
	uint8_t index;

	/* Generar la condici�n de start */
	salida = TWI0_Master_Start_Condition();
	/* Transmitir direcci�n del esclavo en modo escritura */
	salida = TWI0_Master_Tx_Address(slave_addr, TWI_Addr_Write_Mode);
	/* Transmitir la posici�n de memoria en donde se desea empezar a escribir */
	mem_address[0] = ((uint16_t)mem_addr >> 8) & 0xFF;
	mem_address[1] = mem_addr & 0xFF;
	salida = TWI0_Master_Tx_Byte(mem_address[0]);
	salida = TWI0_Master_Tx_Byte(mem_address[1]);
	/* Escribir los datos en posiciones consecutivas de memoria */
	for(index = 0; index < n_bytes; index++){
		salida = TWI0_Master_Tx_Byte(*(buffer + index));
		if(salida == I2C_ERROR){
			break;
		}
	}

	TWI0_Master_Stop_Condition();
	
	_delay_ms(10);

	return salida;
}


uint8_t EEPROM_24AAxx_Clear(uint8_t slave_addr, uint16_t mem_addr, uint8_t n_bytes){
	
	uint8_t mem_address[2];
	uint8_t salida = I2C_OK;
	uint8_t index;

	/* Generar la condici�n de start */
	salida = TWI0_Master_Start_Condition();
	/* Transmitir direcci�n del esclavo en modo escritura */
	salida = TWI0_Master_Tx_Address(slave_addr, TWI_Addr_Write_Mode);
	/* Transmitir la posici�n de memoria en donde se desea empezar a escribir */
	mem_address[0] = (mem_addr >> 8) & 0xFF;
	mem_address[1] = mem_addr & 0xFF;
	salida = TWI0_Master_Tx_Byte(mem_address[0]);
	salida = TWI0_Master_Tx_Byte(mem_address[1]);
	/* Escribir los datos en posiciones consecutivas de memoria */
	for(index = 0; index < n_bytes; index++){
		salida = TWI0_Master_Tx_Byte(0);
		if(salida == I2C_ERROR){
			break;
		}
	}

	TWI0_Master_Stop_Condition();
	
	_delay_ms(10);

	return salida;	

}




/****************************************************************************
* Nombre de la funci�n: EEPROM_Read
* retorna : el estado del bus I2C/TWI (si hubo alg�n error en la transmisi�n)
* arg : slave_addr (direcci�n de esclavo del chip de memoria)
* arg : mem_addr (ubicaci�n de memoria donde se desea empezar a leer)
* arg : n_bytes (cantidad de bytes que se desean leer en la memoria)
* arg: buffer (puntero que indica el arreglo donde se almacenan los datos
*			   que ser�n le�dos)
* Descripci�n : Recibe como par�metro la direcci�n de esclavo del chip de
* 				memoria donde se desea almacenar los datos. Los 'n' bytes que
* 				se desean leer se almacenar�n dentro del arreglo indicado por
* 				el par�metro buffer. Estos datos se empezar�n a leer a partir
* 				de la ubicaci�n de memoria mem_addr.
* Notas : En estos dispositivos, la ubicaci�n de memoria (inicializada en
*  		  mem_addr) se incrementa de forma autom�tica cada vez que se lee
*  		  un dato. SIN EMBARGO, esto solo se cumple en direcciones agrupadas
*  		  en p�ginas de 32 ubicaciones. Por ejemplo: De 0 a 31 (P�gina 0),
*  		  de 32 a 63 (P�gina 1), de 64 a 95 (P�gina 2), etc.
*  		  Si se inicializa la ubicaci�n de memoria con mem_addr = 30 y se
*  		  leen 5 bytes, el incremento autom�tico solo funcionar� hasta la
*  		  ubicaci�n 31, por lo cual esta funci�n no trabajar� correctamente.
*****************************************************************************/
uint8_t EEPROM_24AAxx_Read(uint8_t slave_addr, uint16_t mem_addr, uint8_t n_bytes, uint8_t *buffer){

	uint8_t mem_address[2];
	uint8_t salida = I2C_OK;

	/* Transmitir la posici�n de memoria en donde se desea empezar a leer */
	mem_address[0] = (mem_addr >> 8) & 0xFF;
	mem_address[1] = mem_addr & 0xFF;
	salida = TWI0_Master_Write_Data(slave_addr, 2, mem_address);
	/* Leer los datos en posiciones consecutivas de memoria */
	salida = TWI0_Master_Read_Data(slave_addr, n_bytes, buffer);
	
	_delay_ms(10);

	return salida;
}


uint8_t EEPROM_24AAxx_Write_Extended(uint8_t slave_addr, uint16_t mem_addr, uint16_t n_bytes, uint8_t *buffer){

	uint8_t salida = I2C_OK;
	
	uint16_t n_bytes_left = 0;
	uint16_t n_bytes_1 = 0, n_bytes_2 = 0;
	uint16_t n_pages = 0;
	int16_t index;
	uint16_t offset = 0;
	uint8_t *buffer_ptr = 0;
	uint16_t new_mem_addr = 0;

	/* Calculate number of bytes of the first segment */
	if((mem_addr % EEPROM_PAGE_SIZE_BYTES) == 0){
		n_bytes_1 = 0;
	}else{
		n_bytes_1 = EEPROM_PAGE_SIZE_BYTES - (mem_addr % EEPROM_PAGE_SIZE_BYTES);
	}
	
	if(n_bytes_1 > n_bytes){
		n_bytes_1 = n_bytes;
	}
	
	n_bytes_left = n_bytes - n_bytes_1;
	
	/* Determine if data will be loaded in more than one page */ 
	if(n_bytes_left == 0){
		n_pages = 0;
		n_bytes_2 = 0;
	}else{
		n_pages = n_bytes_left / EEPROM_PAGE_SIZE_BYTES;
		n_bytes_2 = n_bytes_left % EEPROM_PAGE_SIZE_BYTES;
	}

	/* Pointer value at the beginning of the writing */
	buffer_ptr = buffer;
	new_mem_addr = mem_addr;
	
	/* Load the first segment (if it is the case) */
	if(n_bytes_1 != 0){
		salida = EEPROM_24AAxx_Write(slave_addr, new_mem_addr, n_bytes_1, buffer_ptr);
		if(salida == I2C_ERROR){
			return salida;
		}	
		buffer_ptr += n_bytes_1;
		new_mem_addr += n_bytes_1;
	}

	/* Load an integer number of pages */
	for(index = 0; index < n_pages; index++){
		salida = EEPROM_24AAxx_Write(slave_addr, new_mem_addr + offset, EEPROM_PAGE_SIZE_BYTES, buffer_ptr + offset);
		offset += EEPROM_PAGE_SIZE_BYTES;
		if(salida == I2C_ERROR){
			break;
		}
	}

	if(salida == I2C_ERROR){
		return salida;
	}
		
	buffer_ptr += offset;
	new_mem_addr += offset;
		
		
	if(n_bytes_2 != 0){
		salida = EEPROM_24AAxx_Write(slave_addr, new_mem_addr, n_bytes_2, buffer_ptr);
	}
	
	return salida;
}



uint8_t EEPROM_24AAxx_Clear_Extended(uint8_t slave_addr, uint16_t mem_addr, uint16_t n_bytes){

	uint8_t salida = I2C_OK;
	
	uint16_t n_bytes_left = 0;
	uint16_t n_bytes_1 = 0, n_bytes_2 = 0;
	uint16_t n_pages = 0;
	int16_t index;
	uint16_t offset = 0;
	uint16_t new_mem_addr = 0;

	/* Calculate number of bytes of the first segment */
	if((mem_addr % EEPROM_PAGE_SIZE_BYTES) == 0){
		n_bytes_1 = 0;
	}else{
		n_bytes_1 = EEPROM_PAGE_SIZE_BYTES - (mem_addr % EEPROM_PAGE_SIZE_BYTES);
	}
	
	if(n_bytes_1 >= n_bytes){
		n_bytes_1 = n_bytes;
	}
	
	n_bytes_left = n_bytes - n_bytes_1;
	
	/* Determine if data will be loaded in more than one page */
	if(n_bytes_left == 0){
		n_pages = 0;
		n_bytes_2 = 0;
	}else{
		n_pages = n_bytes_left / EEPROM_PAGE_SIZE_BYTES;
		n_bytes_2 = n_bytes_left % EEPROM_PAGE_SIZE_BYTES;
	}

	/* Pointer value at the beginning of the writing */
	new_mem_addr = mem_addr;
	
	/* Load the first segment (if it is the case) */
	if(n_bytes_1 != 0){
		salida = EEPROM_24AAxx_Clear(slave_addr, mem_addr, n_bytes_1);
		if(salida == I2C_ERROR){
			return salida;
		}
		new_mem_addr += n_bytes_1;
	}

	/* Load an integer number of pages */
	for(index = 0; index < n_pages; index++){
		salida = EEPROM_24AAxx_Clear(slave_addr, new_mem_addr + offset, EEPROM_PAGE_SIZE_BYTES);
		offset += EEPROM_PAGE_SIZE_BYTES;
		if(salida == I2C_ERROR){
			break;
		}
	}

	if(salida == I2C_ERROR){
		return salida;
	}
	
	new_mem_addr += offset;
	
	
	if(n_bytes_2 != 0){
		salida = EEPROM_24AAxx_Clear(slave_addr, new_mem_addr, n_bytes_2);
	}
	
	return salida;
}



uint8_t EEPROM_24AAxx_Read_Extended(uint8_t slave_addr, uint16_t mem_addr, uint16_t n_bytes, uint8_t *buffer){

	uint8_t salida = I2C_OK;
	
	uint16_t n_bytes_left = 0;
	uint16_t n_bytes_1 = 0, n_bytes_2 = 0;
	uint16_t n_pages = 0;
	int16_t index;
	uint16_t offset = 0;
	uint8_t *buffer_ptr = 0;
	uint16_t new_mem_addr = 0;

	/* Calculate number of bytes of the first segment */
	if((mem_addr % EEPROM_PAGE_SIZE_BYTES) == 0){
		n_bytes_1 = 0;
	}else{
		n_bytes_1 = EEPROM_PAGE_SIZE_BYTES - (mem_addr % EEPROM_PAGE_SIZE_BYTES);
	}
	
	if(n_bytes_1 >= n_bytes){
		n_bytes_1 = n_bytes;
	}
	
	n_bytes_left = n_bytes - n_bytes_1;
	
	/* Determine if data will be loaded in more than one page */
	if(n_bytes_left == 0){
		n_pages = 0;
		n_bytes_2 = 0;
	}else{
		n_pages = n_bytes_left / EEPROM_PAGE_SIZE_BYTES;
		n_bytes_2 = n_bytes_left % EEPROM_PAGE_SIZE_BYTES;
	}

	/* Pointer value at the beginning of the writing */
	buffer_ptr = buffer;
	new_mem_addr = mem_addr;
	
	/* Load the first segment (if it is the case) */
	if(n_bytes_1 != 0){
		salida = EEPROM_24AAxx_Read(slave_addr, mem_addr, n_bytes_1, buffer);
		if(salida == I2C_ERROR){
			return salida;
		}
		buffer_ptr += n_bytes_1;
		new_mem_addr += n_bytes_1;
	}

	/* Load an integer number of pages */
	for(index = 0; index < n_pages; index++){
		salida = EEPROM_24AAxx_Read(slave_addr, new_mem_addr + offset, EEPROM_PAGE_SIZE_BYTES, buffer_ptr  + offset);
		offset += EEPROM_PAGE_SIZE_BYTES;
		if(salida == I2C_ERROR){
			break;
		}
	}

	if(salida == I2C_ERROR){
		return salida;
	}
	
	buffer_ptr += offset;
	new_mem_addr += offset;
	
	
	if(n_bytes_2 != 0){
		salida = EEPROM_24AAxx_Read(slave_addr, new_mem_addr, n_bytes_2, buffer_ptr);
	}
	
	return salida;
}






