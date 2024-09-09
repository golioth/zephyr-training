<!-- Copyright (c) 2024 Golioth, Inc. -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.4.0] 2024-10-29

### Changed
- Use Golioth Firmware SDK v0.15.0
- Update board names for Zephyr hardware model v2
- Use zephyr-network-info v1.2.1
- Update board names for hardware model v2
- Use `socs` folder for chip-specific configuration
- Remove application-specific WiFi manager and use the one in Golioth common library
- Pare down manifest allow-list to remove unused packages
- Add GitHub actions workflows for build and release

## [2.3.0] 2024-06-25

### Changed
- Use Golioth Firmware SDK v0.14.0

## [2.2.0] 2024-06-12

### Changed
- Use Golioth Firmware SDK v0.13.1
- Bump PSK-ID/PSK length limit from 64 to 128
- Use CBOR to send temperature stream data to match default pipeline expectation

## [2.1.0] 2024-04-23

### Changed
- Use Golioth Firmware SDK v0.12.0
- Add Golioth Firmware SDK dependencies to prj.conf
- Update network info to v1.1.1

### Fixed
- Network Info RPC is no longer truncated with nRF9160dk

## [2.0.0] 2024-02-16

### Added
- Changelog
- Code of Conduct

### Changed
- Use Golioth Firmware SDK (at v0.10.0)
