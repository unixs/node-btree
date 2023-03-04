NAMESPACE=unixs
IMAGE=node-btree-testing
REPO=$(NAMESPACE)/$(IMAGE)

LEGACY_TAG=legacy
LATEST_TAG=latest

LEGACY_FILE=docker/ubuntu-$(LEGACY_TAG)-tests.dockerfile
LATEST_FILE=docker/ubuntu-$(LATEST_TAG)-tests.dockerfile

PWD:=$(shell pwd)
WORKDIR=/home/developer/dev
VOLUME=$(PWD):$(WORKDIR)

BUILD_DIR=build

COVERAGE_DIR=coverage
COVERAGE_REPORT=$(COVERAGE_DIR)/index.html

default_target: docker-build-all
.PHONY : default_target


rebuild-debug:
	npx cmake-js rebuild -D
.PHONY : rebuild-debug

rebuild:
	npx cmake-js rebuild
.PHONY : rebuild

build-debug:
	npx cmake-js build -D
.PHONY : build-debug

build:
	npx cmake-js build
.PHONY : build


docker-build-latest:
	docker build -t $(REPO):$(LATEST_TAG) -f $(LATEST_FILE) .
.PHONY : docker-build-latest

docker-build-legacy:
	docker build -t $(REPO):$(LEGACY_TAG) -f $(LEGACY_FILE) .
.PHONY : docker-build-legacy

docker-build: docker-build-legacy docker-build-latest
.PHONY : docker-build


docker-push-latest: docker-build-latest
	docker push $(REPO):$(LATEST_TAG)
.PHONY : docker-push-latest

docker-push-legacy: docker-build-legacy
	docker push $(REPO):$(LEGACY_TAG)
.PHONY : docker-push-legacy

docker-push: docker-push-latest docker-push-legacy
.PHONY : docker-push


docker-test-latest: docker-build-latest
	docker run -it --rm -h $(LATEST_TAG) -v $(VOLUME) $(REPO):$(LATEST_TAG)
.PHONY : docker-test-latest

docker-test-legacy: docker-build-legacy
	docker run -it --rm -h $(LEGACY_TAG) -v $(VOLUME) $(REPO):$(LEGACY_TAG)
.PHONY : docker-test-legacy

test-jest: rebuild
	npm test
.PHONY : test-jest

test-jest-debug: rebuild-debug
	NODE_ENV=development npm test
.PHONY : test-jest-debug

docker-test: docker-test-legacy docker-test-latest
.PHONY : docker-test

test: docker-test-legacy docker-test-latest test-jest coverage
.PHONY : test


coverage: test-jest-debug
	cd build && ctest -VV
.PHONY : coverage

coverage-html: clean-coverage test-jest-debug
	mkdir $(COVERAGE_DIR)
	gcovr --print-summary --html --html-details -o $(COVERAGE_REPORT) -r . $(BUILD_DIR)
.PHONY : coverage-html


clean-coverage:
	rm -rf $(COVERAGE_DIR)
.PHONY : clean-coverage

clean: clean-coverage
	rm -rvf $(BUILD_DIR) Testing
.PHONY : clean


install-deps:
	npm ci
.PHONY : install-deps
