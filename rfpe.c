#include "ruby.h"

VALUE cRFPE;

static VALUE
rfpe_encrypt(VALUE self, VALUE message, VALUE regex, VALUE key) {
}

void Init RFPE() {
  cRFPE = rb_define_class("RFPE", rb_cObject);
  rb_define_singleton_method(cRFPE, "new", rfpe_new, 0);
  rb_define_method(cRFPE, "encrypt", rfpe_encrypt, 
}
