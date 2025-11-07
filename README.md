<h1 align="center">HiveMnd Utility Library</h1>
<h3 align="center">Header-only C++ Utility Library by HiveMnd LLC (Jonathan Smith)</h3>

<p align="center">
  <a href="#"><img src="https://img.shields.io/badge/language-C%2B%2B-blue.svg?style=flat-square"></a>
  <a href="#"><img src="https://img.shields.io/badge/license-Proprietary-yellow.svg?style=flat-square"></a>
  <a href="#"><img src="https://img.shields.io/badge/maintained%20by-HiveMnd%20LLC-black.svg?style=flat-square"></a>
</p>

---

## Overview

The **HiveMnd Utility Library** is a professional, header-only C++ library containing reusable, production-ready helper functions.
It was originally developed as a systems programming lab foundation and has evolved into a multi-purpose codebase for internal and external HiveMnd projects.

This library promotes **readability**, **efficiency**, and **reusability**, allowing developers to quickly implement robust functionality without reinventing the wheel.

---

## Features

*  Input validation and data conversion
*  Console control (color, clear, pause, buffer management)
*  Math & numeric helpers (clamp, random range, safe divide, etc.)
*  String and character utilities
*  Debugging and developer tools
*  Header-only — no linking or compiling required

---

## Installation

Clone the repository:

```bash
git clone https://github.com/jts4110/HiveMnd-UtilityLibrary.git
```

Include the header file in your project:

```cpp
#include "Utility.h"
```

Access functions through the namespace:

```cpp
HiveMnd::Utility
```

---

## Example Usage

```cpp
#include "Utility.h"
#include <iostream>

int main()
{
    int number = 42;

    HiveMnd::Utility::PrintIntegerBinary(&number);

    if (HiveMnd::Utility::IsEven(&number))
        std::cout << "The number is even!" << std::endl;

    return 0;
}
```

---

## Directory Structure

```
CodeLibrary/
└── UtilityLib/
    └── HiveMnd_UtilityLibrary/
        └── Utility.h
```

---

## Future Additions

*  File system utilities (file read/write, directory scanning)
*  String trimming, formatting, and parsing
*  Performance timers and profiling tools
*  Logging system
*  Random data and procedural generators

---

## Contribution Guidelines

1. Fork this repository
2. Create a new branch for your feature or bug fix
3. Maintain HiveMnd’s commenting and formatting standards
4. Submit a pull request for review

All merges are subject to HiveMnd LLC approval.

---

## License & Usage

This project is the intellectual property of **HiveMnd LLC**.
Unauthorized commercial use, distribution, or modification without written consent is strictly prohibited.

For permissions, licensing, or collaborations, please contact:
**[hivemndskilz@gmail.com](mailto:hivemndskilz@gmail.com)**

---

## Acknowledgements

Created and maintained by **Jonathan Smith**
Founder and Lead Developer at **HiveMnd LLC**

“Crafted by developers, for developers.”

---

<p align="center">
  <sub>© 2025 HiveMnd LLC | All Rights Reserved</sub>
</p>
