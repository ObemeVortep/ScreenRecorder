# 🖥️ ScreenRecorder – A High-Performance Modern C++ Project

This repository documents a personal project focused on building a **high-performance screen recording system** in modern C++ (C++17/20), using low-level system APIs and high-efficiency libraries.

---

## ✅ Current Capabilities

- ⚡ **Ultra-fast screen capture** via **Desktop Duplication API (DDAPI)**  
  This is the most efficient method available for Windows screen capture. It provides **direct access to GPU memory**, minimizing latency and resource usage.

- ⚡ **High-speed JPEG compression** using **libjpeg-turbo**  
  Raw bitmaps are compressed using one of the **fastest JPEG encoders available** — there is **no faster open-source alternative**.

---

## 🛠 Planned Features

- 🎙️ Capture **system audio and microphone input** (if supported)
- 🖱️ Overlay the **mouse cursor**
- 🌐 **Transmit frames over the network** (e.g. to a server or client)

---

## 🎯 Project Goals

- Apply **idiomatic modern C++** design (RAII, smart pointers, STL containers)
- Build components with **multithreading** and **networking** in mind
- Practice **real-world systems-level design** patterns
- Focus on **performance-first** architecture

---

> 🧪 This project is a hands-on exercise in writing clean, robust, high-performance modern C++ code — intended to go beyond theory and actually build something meaningful.

⏳ **Estimated development time**: ~2–3 weeks of focused work.
