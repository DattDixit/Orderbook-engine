# Orderbook-engine


A high-performance, low-latency orderbook matching engine built in C++ to power algorithmic trading systems. This project implements core trading functionalities with support for various order types, optimized for speed and scalability using modern C++ features.

Overview
This project, started in November 2024, simulates the heart of an electronic trading system—an orderbook matching engine. It manages buy (bid) and sell (ask) orders, matches them based on price-time priority, and executes trades efficiently. Designed with low-latency in mind, it leverages C++17/20, multithreading potential, and precise timing via std::chrono.

Key Features
Order Management: Add, cancel, and modify orders seamlessly.
Order Types: Supports Good-Til-Canceled (GTC), Fill-or-Kill (FOK), Fill-and-Kill (FAK), and Market orders (basic implementation).
Price-Time Priority: Matches orders based on best price, with oldest orders prioritized at the same price level.
Low Latency: Optimized data structures (std::multimap) and nanosecond-precision timestamps (chrono).
Scalability: Built to handle high-throughput trading scenarios.
Prerequisites
Compiler: C++17 or later (e.g., GCC 7+, Clang 5+, MSVC 2017+).
Operating System: Tested on Linux/Windows; should work on macOS with minor adjustments.
Dependencies: None beyond the C++ Standard Library.

#How It Works
Order Addition: Adds a buy order at $100 (10 units), then a sell order at $99.5 (5 units). The engine matches them because $100 ≥ $99.5, trading 5 units at the ask price.
Remaining Orders: The buy order is partially filled (5 units left), and a new buy at $99.5 is added but finds no matching ask.
Data Structures:
std::multimap sorts bids (descending) and asks (ascending).
chrono::nanoseconds ensures time priority via insertion order
