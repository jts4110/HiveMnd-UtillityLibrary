// -----------------------------------------------------------------------------
// HiveMnd Utility Library : NetworkUtils
// -----------------------------------------------------------------------------
// Overview:
//  The NetworkUtils module provides lightweight, cross-platform network helpers
//  for connectivity testing, local IP/hostname resolution, and simple ping tests.
//
//  This layer abstracts low-level socket setup to make diagnostic operations
//  and online feature development easier to prototype and debug.
//
// Key Features:
//  • Retrieve local hostname and IP address
//  • Test basic internet connectivity
//  • Perform lightweight host ping checks
//  • Windows + Linux cross-compatibility
//
// Usage:
//  #include "Network/NetworkUtils.h"
//  std::string ip = HiveMnd::Network::GetLocalIPAddress();
// -----------------------------------------------------------------------------

#pragma once
#include <string>
#include <vector>

namespace HiveMnd::Network
{
    // Returns the current device's hostname.
    std::string GetHostName();

    // Returns the primary local IPv4 address (e.g. "192.168.1.5").
    std::string GetLocalIPAddress();

    // Attempts to detect if the system has internet access.
    // Returns true if at least one external host can be resolved or pinged.
    bool IsInternetAvailable();

    // Performs a lightweight ICMP-style ping test.
    // Returns true if the target host is reachable.
    bool PingHost(const std::string& host);

    // Lists all IPv4 addresses assigned to this device.
    std::vector<std::string> GetAllLocalIPs();
}

