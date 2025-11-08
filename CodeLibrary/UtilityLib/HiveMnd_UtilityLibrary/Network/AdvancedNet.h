// -----------------------------------------------------------------------------
// HiveMnd Utility Library : AdvancedNet
// -----------------------------------------------------------------------------
// Overview:
//  The AdvancedNet module extends NetworkUtils with deeper diagnostic,
//  performance, and encoding utilities. It provides real-time latency checks,
//  hostname lookups, and simple data throughput estimations.
//
//  This layer is designed for developers who need live network performance data
//  without requiring external libraries or full HTTP stacks.
//
// Key Features:
//  • Measure ping latency to a remote host
//  • Perform DNS resolution lookups
//  • Estimate download throughput
//  • Encode/decode small data packets safely
//
// Usage:
//  #include "Network/AdvancedNet.h"
//  double latency = HiveMnd::Network::GetPingLatency("www.google.com");
// -----------------------------------------------------------------------------

#pragma once
#include <string>
#include <vector>

namespace HiveMnd::Network
{
    // Measures average latency (in milliseconds) to the given host using a TCP connection.
    // Returns -1 if the host cannot be reached.
    double GetPingLatency(const std::string& host, int port = 80, int attempts = 3);

    // Resolves a domain name (e.g., "example.com") to a list of IP addresses.
    std::vector<std::string> DNSLookup(const std::string& hostname);

    // Performs a reverse lookup: converts an IP (e.g., "8.8.8.8") back to its domain name.
    std::string ReverseDNS(const std::string& ip);

    // Performs a minimal HTTP GET request and estimates the download speed (KB/s).
    // Optional: limit test duration or bytes downloaded.
    double MeasureDownloadSpeed(const std::string& url, size_t maxBytes = 1024 * 512);

    // Encodes binary data to a simple Base64-like string.
    std::string EncodeData(const std::vector<unsigned char>& data);

    // Decodes data previously encoded with EncodeData.
    std::vector<unsigned char> DecodeData(const std::string& encoded);
}

