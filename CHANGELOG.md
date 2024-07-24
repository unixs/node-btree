# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

* INT btree without ES comparator fun-on

## [0.9.0] - 2024-07-24

### Changed

* refactoring

### Added

* types declarations

### Fixed

* clear() method bug

## [0.8.0] - 2023-03-02

### Changed

* Lot of changes in tests
* Bugfix for building fail for GLIB < 2.68

### Added

* toArray()
* toArrays()
* flatten()
* getKeys()
* getValues()
* Legacy GLIB tests

## [0.7.0] - 2023-02-27

### Changed

* Building system was changed from gyp to CMake!
* Iterators (Symbol.iterator) and related features now only for GLib < 2.68
* toMap(), toSet() only for GLib < 2.68

### Added

* forEachReverse()
* last()
* first()
* before()
* after()
* between()
* GLIB_VERSION exported object
* GLIB_VERSION.MAJOR number field
* GLIB_VERSION.MINOR number field
* GLIB_VERSION.PATCH number field
* GLIB_VERSION.hasGTreeNode() predicate - returns true if GLib >= 2.68
* Coverage test report for C code

## [0.6.2] - 2022-06-06

### Changed

* MacOS building was enabled

## [0.6.1] - 2022-02-05

### Changed

* Dependencies updates

## [0.6.0] - 2020-07-05

### Changed

* Small refactoring

### Added

* toMap()
* toSet()

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
