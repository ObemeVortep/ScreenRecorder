# 🖥️ ScreenRecorder – A High-Performance Modern C++ Project

This repository documents a personal project focused on building a **high-performance screen-recording system** in modern C++ (C++17/20), using low-level system APIs and high-efficiency libraries.

---

## ✅ Current Capabilities

* ⚡ **GPU-only screen capture pipeline**  
  Uses **Desktop Duplication API (DDAPI)** to read the frame buffer **directly from GPU memory**, then leverages **DX11On12 interop** to pass that texture straight into **NVIDIA NVENC**. The entire path - capture -> transfer -> H.264 encode - happens **without touching the CPU**, delivering the highest possible throughput and the lowest latency.

* ⚡ **Ultra-fast screen capture (DDAPI)**  
  DDAPI remains the most efficient capture method on Windows and is the backbone of the pipeline above.

* 🎧 **System audio and microphone capture (WASAPI)**  
  Supports parallel recording of **loopback (system audio)** and **microphone input** using **WASAPI**, with a unified audio interface.  
  > 🔜 Upcoming: audio streams will be **mixed** and **encoded to AAC** for efficient muxing and streaming.

* ⚡ **High-speed JPEG compression** with **libjpeg-turbo**  
  Raw bitmaps can still be compressed to JPEG using one of the fastest open-source encoders available.

---

## 🛠 Planned Features

* 🎙️ **Mix and encode audio tracks to AAC**  
  Combine system and mic audio into a single stream, encoded with a low-latency AAC encoder (e.g. FDK-AAC or Media Foundation AAC).

* 🖱️ **Overlay the mouse cursor**  
  Capture and render the cursor position on top of the video frames in real time.

* 📦 **MP4 container muxing**  
  Combine H.264 video and AAC audio into a standard MP4 file for playback or upload.

* 🌐 **Transmit frames over the network**  
  Enable real-time streaming via RTP, RTSP, or WebRTC pipelines.

---

## ⚙️ Requirements

* **Windows 10 20H2** or newer (required for DirectX 12)
* **NVIDIA GPU** with driver **version 570+** (required for NVIDIA NVENC 13.0)

---

## 🎯 Project Goals

* **Match or outperform leading recorders** such as **GeForce Experience** and **OBS Studio** by minimizing copy‑paths, cache misses and driver overhead; the lightweight core should translate into **lower CPU/GPU utilization and latency** than those heavyweight tools.

* Apply **idiomatic modern C++** throughout (RAII, `std::unique_ptr`, STL containers) to keep the code safe and easy to audit.

* **Prioritize maximum performance** across the entire pipeline - early benchmarks already show **better efficiency than GeForce Experience**, with lower system overhead and faster response times.

* Keep the codebase **clean, modular, and reliable**, following modern C++ best practices to ensure long-term maintainability and ease of extension.

---

> 🧪 This project is a hands‑on exercise in writing clean, robust, high‑performance modern C++ code - intended to go beyond theory and actually build something meaningful.

⏳ **Estimated development time**: \~2-3 weeks of focused work.
