# Variables
DOXYGEN_CONFIG = Doxyfile
DOXYGEN_OUTPUT_DIR = out
HTML_DIR = $(DOXYGEN_OUTPUT_DIR)/html
ZIP_FILE = docs.zip

all: doxygen

doxygen:
	doxygen $(DOXYGEN_CONFIG)

zip: doxygen
	cd $(HTML_DIR) && zip -r ../../$(ZIP_FILE) ./*
	@echo "Zipped $(HTML_DIR) contents to $(ZIP_FILE)"

clean:
	rm -rf $(DOXYGEN_OUTPUT_DIR) $(ZIP_FILE)

.PHONY: all doxygen zip clean
