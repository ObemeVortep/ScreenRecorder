# 🖥️ ScreenRecorder – A High-Performance Modern C++ Project

This repository documents a personal project focused on building a **high-performance screen-recording system** in modern C++ (C++17/20), using low-level system APIs and high-efficiency libraries.

---

## ✅ Current Capabilities

* ⚡ **GPU-only screen capture pipeline**
  Uses **Desktop Duplication API (DDAPI)** to read the frame buffer **directly from GPU memory**, then leverages **DX11On12 interop** to pass that texture straight into **NVIDIA NVENC**. The entire path — capture → transfer → H.264 encode — happens **without touching the CPU**, delivering the highest possible throughput and the lowest latency.

* ⚡ **Ultra-fast screen capture (DDAPI)**
  DDAPI remains the most efficient capture method on Windows and is the backbone of the pipeline above.

* ⚡ **High-speed JPEG compression** with **libjpeg-turbo**
  Raw bitmaps can still be compressed to JPEG using one of the fastest open-source encoders available.

---

## 🛠 Planned Features

* 🎙️ Capture **system audio and microphone input** (if supported)
* 🖱️ Overlay the **mouse cursor**
* 🌐 **Transmit frames over the network** (e.g. to a server or client)

---

## ⚙️ Requirements

* **Windows 10 20H2** or newer (required for DirectX 12)
* **NVIDIA GPU** with driver **version 570+** (required for NVIDIA NVENC 13.0)

---

## 🎯 Project Goals

* **Match or outperform leading recorders** such as **GeForce Experience** and **OBS Studio** by minimizing copy‑paths, cache misses and driver overhead; the lightweight core should translate into **lower CPU/GPU utilization and latency** than those heavyweight tools.
* Apply **idiomatic modern C++** throughout (RAII, `std::unique_ptr`, STL containers) to keep the code safe and easy to audit.
* Design every component with **zero‑copy GPU data‑flow** and **lock‑free / wait‑free paths** where feasible; exploit fine‑grained multithreading to saturate modern CPUs without contention.
* Keep the codebase **modular & portable** so it can be embedded into game engines or integrated as a plug‑in in larger multimedia stacks.
* Provide extension points for **real‑time streaming and networking**, enabling easy RTP/RTSP/WebRTC add‑ons in future.

---

> 🧪 This project is a hands‑on exercise in writing clean, robust, high‑performance modern C++ code — intended to go beyond theory and actually build something meaningful.

⏳ **Estimated development time**: \~2–3 weeks of focused work.
