

#if STM32F == 2
#include "stm32f2xx.h"
#endif
#if STM32F == 4
#include "stm32f4xx.h"
#endif

#include "spi.h"
#include "main.h"

#define SPIx_SD                         SPI1
#define SPIx_SD_CLK                     RCC_APB2Periph_SPI1
#define SPIx_SD_CLK_INIT                RCC_APB2PeriphClockCmd
#define SPIx_SD_IRQn                    SPI1_IRQn
#define SPIx_SD_IRQHANDLER              SPI1_IRQHandler

#define SPIx_SD_SCK_PIN                 GPIO_Pin_5
#define SPIx_SD_SCK_GPIO_PORT           GPIOA
#define SPIx_SD_SCK_GPIO_CLK            RCC_AHB1Periph_GPIOA
#define SPIx_SD_SCK_SOURCE              GPIO_PinSource5
#define SPIx_SD_SCK_AF                  GPIO_AF_SPI1

#define SPIx_SD_MOSI_PIN                GPIO_Pin_7
#define SPIx_SD_MOSI_GPIO_PORT          GPIOA
#define SPIx_SD_MOSI_GPIO_CLK           RCC_AHB1Periph_GPIOA
#define SPIx_SD_MOSI_SOURCE             GPIO_PinSource7
#define SPIx_SD_MOSI_AF                 GPIO_AF_SPI1


#define SPIx_SD_BAUDRATE_SLOW  			SPI_BaudRatePrescaler_4
//#define SPIx_SD_BAUDRATE_FAST  			SPI_BaudRatePrescaler_8



/*#define SPI_SD                   SPI2
#define GPIO_CS                  GPIOC
#define RCC_APB2Periph_GPIO_CS   RCC_AHB1Periph_GPIOB
#define GPIO_Pin_CS              GPIO_Pin_7
#define DMA_Channel_SPI_SD_RX    DMA1_Channel2
#define DMA_Channel_SPI_SD_TX    DMA1_Channel3
#define DMA_FLAG_SPI_SD_TC_RX    DMA1_FLAG_TC2
#define DMA_FLAG_SPI_SD_TC_TX    DMA1_FLAG_TC3
#define GPIO_SPI_SD              GPIOB
#define GPIO_Pin_SPI_SD_SCK      GPIO_Pin_10
#define GPIO_Pin_SPI_SD_MISO     GPIO_Pin_14
A
#define GPIO_Pin_SPI_SD_MOSI     GPIO_Pin_15
#define RCC_APBPeriphClockCmd_SPI_SD  RCC_APB1PeriphClockCmd
#define RCC_APBPeriph_SPI_SD     RCC_APB1Periph_SPI2
*/

/* - for SPI1 and full-speed APB2: 72MHz/4 */
//#define SPI_BaudRatePrescaler_SPI_SD  SPI_BaudRatePrescaler_4

void spi_send( uint8_t out )
{
	while (SPI_I2S_GetFlagStatus(SPIx_SD, SPI_I2S_FLAG_TXE) == RESET) { ; }
	SPI_I2S_SendData(SPIx_SD, out);
}

void init_spi(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(SPIx_SD_SCK_GPIO_CLK | SPIx_SD_MOSI_GPIO_CLK, ENABLE);

	/* Enable the SPI clock */
	SPIx_SD_CLK_INIT(SPIx_SD_CLK, ENABLE);

	
	/* SPI GPIO Configuration --------------------------------------------------*/

	/* Connect SPI pins to AF5 */
	GPIO_PinAFConfig(SPIx_SD_SCK_GPIO_PORT, SPIx_SD_SCK_SOURCE, SPIx_SD_SCK_AF);
	GPIO_PinAFConfig(SPIx_SD_MOSI_GPIO_PORT, SPIx_SD_MOSI_SOURCE, SPIx_SD_MOSI_AF);

	/* SPI SCK pin configuration */
	GPIO_InitStructure.GPIO_Pin = SPIx_SD_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(SPIx_SD_SCK_GPIO_PORT, &GPIO_InitStructure);

	/* SPI  MOSI pin configuration */
	GPIO_InitStructure.GPIO_Pin =  SPIx_SD_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_Init(SPIx_SD_MOSI_GPIO_PORT, &GPIO_InitStructure);

	/* SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPIx_SD_BAUDRATE_SLOW; // 42000kHz/128=328kHz < 400kHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_CalculateCRC(SPI1, DISABLE);
	SPI_Cmd(SPI1, ENABLE);

}


