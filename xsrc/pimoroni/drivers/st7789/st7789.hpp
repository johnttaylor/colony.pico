#pragma once

#include "common/pimoroni_common.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "Driver/SPI/Master.h"

// Platform independent, SPI only driver


namespace pimoroni {

  class ST7789 : public DisplayDriver {
    Driver::SPI::Master& spi;
  
  public:
    bool round;

    //--------------------------------------------------
    // Variables
    //--------------------------------------------------
  private:

    // interface pins with our standard defaults where appropriate
    uint cs;
    uint dc;
    uint bl;


    // The ST7789 requires 16 ns between SPI rising edges.
    // 16 ns = 62,500,000 Hz
    static const uint32_t SPI_BAUD = 62'500'000;


  public:

    // Serial init
    ST7789(uint16_t width, uint16_t height, Rotation rotation, bool round, SPIPins pins) :
      DisplayDriver(width, height, rotation),
      spi(pins.spi), round(round),
      cs(pins.cs), dc(pins.dc), wr_sck(pins.sck), d0(pins.mosi), bl(pins.bl) {

      // configure spi interface and pins
      spi_init(spi, SPI_BAUD);

      gpio_set_function(wr_sck, GPIO_FUNC_SPI);
      gpio_set_function(d0, GPIO_FUNC_SPI);

      common_init();
    }

    void cleanup() override;
    void update(PicoGraphics *graphics) override;
    void set_backlight(uint8_t brightness) override;

  private:
    void common_init();
    void configure_display(Rotation rotate);
    void command(uint8_t command, size_t len = 0, const char *data = NULL);
  };

}
