/* Exceptions Header File */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "lib.h"

/* List of Exceptions */
// Vector number 0
extern void exception_divbyzero();

// Vector number 1
extern void exception_debug();

// Vector number 2
extern void exception_nmi();

// Vector number 3
extern void exception_breakpoint();

// Vector number 4
extern void exception_overflow();

// Vector number 5
extern void exception_range();

// Vector number 6
extern void exception_invalidopcode();

// Vector number 7
extern void exception_device();

// Vector number 8
extern void exception_doublefault();

// Vector number 9
extern void exception_coprocessor();

// Vector number 10
extern void exception_invalidtss();

// Vector number 11
extern void exception_segmentnotpresent();

// Vector number 12
extern void exception_stacksegment();

// Vector number 13
extern void exception_generalprotection();

// Vector number 14
extern void exception_pagefault();

// Vector number 15
extern  void exception_reserved();

// Vector number 16
extern void exception_floatingpoint();

// Vector number 17
extern void exception_alignmentcheck();

// Vector number 18
extern void exception_machinecheck();

// Vector number 19
extern void exception_simdfloating();

// Vector number 20
extern void exception_virtualization();

// Vector number 30
extern void exception_security();

#endif
