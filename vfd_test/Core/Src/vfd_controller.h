#ifndef __VFD_CONTROLLER_H__
#define __VFD_CONTROLLER_H__

#include <stdint.h>

typedef struct {
  void(*prepare_write)();
  void(*finish_write)();
  void(*write_byte)(uint8_t);
} vfd_serial;

// TODO: remove from public interface?
void vfd_serial_write(const vfd_serial* s, const uint8_t* data, uint8_t size);

typedef struct {
  const vfd_serial* serial;
  uint8_t max_digits;
  uint8_t max_segments;
} vfd_controller;

// doesn't write data to the controller, does only in-memory changes
// digit/segment counting starts from 1, not 0!
void vfd_enable_segment(const vfd_controller* c, uint8_t buf[],
                        uint8_t dig, uint8_t seg);
void vfd_disable_segment(const vfd_controller* c, uint8_t buf[],
                         uint8_t dig, uint8_t seg);

void vfd_write_cmd(const vfd_controller* c, uint8_t cmd);

void vfd_write_byte(const vfd_controller* c, uint8_t addr, uint8_t byte);
void vfd_write_data(const vfd_controller* c, uint8_t addr,
                    const uint8_t* data, uint8_t size);

void vfd_common_init(const vfd_controller* c, uint8_t displ_cfg);

#endif /* __VFD_CONTROLLER_H__ */
