# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

* toArray()
* toArrays()
* toObject()
* toJSON()

## [0.5.3] - 2020-07-05

### Changed

* C code was splitted  in to separate files

## [0.5.2] - 2020-01-23

### Added

* Creating new instances without "new" keyword.

### Fixed

* Unexpected random segfault (on GC)

## [0.5.1] - 2019-11-11

### Changed

* README.md (add documentation link)

## [0.5.0] - 2019-11-07

### Added

* filter() method
* BTree.from() static method

## [0.4.0] - 2019-10-31

### Added

* reduce() method
* reduce() tests

### Changed

* README.md

## [0.3.3] - 2019-10-30

### Added

* CHANGELOG.md
* map() method
* map() tests

### Changed

* forEach(): Callback arguments order ( key, val -> to -> val, key)
* forEach(): Added second argument - callback context
* forEach() tests

## [0.3.2] - 2019-10-24

Big Bang!
