SRC = demo.tex
OUT = demo.pdf

# dangerous to call make on the same file with the paths coded here
BUILD = .build
CDIR = $(shell pwd)
BDIR = $(CDIR)/$(BUILD)

TEX = latexmk
MKDIR = mkdir -p
RSYNC = rsync 
RFLAGS = --checksum --recursive --exclude=$(BUILD) --exclude=.git --exclude=./*.pdf --exclude=.gitignore 

all: 
	@echo $(CDIR)
	$(MKDIR) $(BDIR)
	$(RSYNC) $(RFLAGS) $(CDIR)/ $(BDIR)
	$(MAKE) -C $(BUILD) tex

tex:
	$(TEX) $(SRC) 
	@cp $(OUT) ../  # hack -- c.f. paths above

clean:
	@rm -rf $(BDIR)
	@rm -rf $(OUT)

