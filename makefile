CC =gcc
CFLAGS = -g -Wall
OBJPATH = obj/
TGTPATH = bin/
OBJ = $(OBJPATH)io.o  $(OBJPATH)request.o  $(OBJPATH)error.o  $(OBJPATH)http.o  $(OBJPATH)cgi.o
TGT = $(TGTPATH)http
SILENCE = @

.PHONY:all
all:$(TGT)

$(TGT):|$(TGTPATH)

$(TGTPATH):
	mkdir -p $(TGTPATH)

$(TGT):$(OBJ)
	$(CC) -o $(TGT) $(CFLAGS) $(OBJ)

$(OBJ):|$(OBJPATH)

$(OBJPATH):
	mkdir -p $(OBJPATH)

$(OBJPATH)%.o:%.c
	$(CC) -o $@ -c $(CFLAGS) $^

.PHONY:clean
clean:
	@echo cleanning...
	$(SILENCE)rm -rf $(OBJPATH)
	$(SILENCE)rm -rf $(TGTPATH)
	#$(SILENCE)rmdir $(OBJPATH)
	#$(SILENCE)rmdir $(TGTPATH)
