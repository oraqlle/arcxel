# Contributing

## Header Structure

* Headers (`*.h`) should only contain interfaces unless they are templates
* Source files (`*.cxx`) contain implementation
* Code on a measured hot path may also be defined inline in a header

### The hot path exception

A function defined in a `.cxx` cannot be inlined into a call site in another
translation unit, as the project does not build with link-time optimisation.
For code that runs once per measurement, that call is recorded as part of
whatever is being timed.

`arcxel::timing::Span` and `record()` are the current cases, along with the
sample storage they touch. Everything cold, meaning label registration, the
summary and file output, stays in a `.cxx`.

Take the exception only where a measurement justifies it, and say why in the
header. It is not a general licence to write header-only code.

```cxx
// <[header_name].h> -*- C++ -*-

//  Arcxel Test Bench
//  Copyright (C) 2026  Tyler Swann, Georgia Kanellis
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License v2.1 as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
//  USA

#ifndef ARCXEL_<HEADER_NAME>_H
#define ARCXEL_<HEADER_NAME>_H

#include <> // internal arcxel headers

#include <> // third party headers

#include <> // standard headers

namespace arcxel {

// ... header content ...

} // namespace arcxel

#endif // ARCXEL_<HEADER_NAME>_H
```

## Source Files

```cxx
// <[source_name].cxx> -*- C++ -*-

//  Arcxel Test Bench
//  Copyright (C) 2026  Tyler Swann, Georgia Kanellis
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License v2.1 as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
//  USA

#include <> // matching header

#include <> // internal arcxel header

#include <> // third party headers

#include <> // standard headers

namespace arcxel {

// ... source implementations ...

} // namespace arcxel
```
