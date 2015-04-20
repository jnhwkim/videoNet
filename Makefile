# Location of the CUDA Toolkit
CUDA_PATH       ?= /Developer/NVIDIA/CUDA-7.0

# architecture
HOST_ARCH   := $(shell uname -m)
TARGET_ARCH ?= $(HOST_ARCH)
ifneq (,$(filter $(TARGET_ARCH),x86_64 aarch64 ppc64le))
    TARGET_SIZE := 64
else ifeq ($(TARGET_ARCH),armv7l)
    TARGET_SIZE := 32
else
    $(error ERROR - unsupported value $(TARGET_ARCH) for TARGET_ARCH!)
endif
ifneq ($(TARGET_ARCH),$(HOST_ARCH))
    ifeq (,$(filter $(HOST_ARCH)-$(TARGET_ARCH),aarch64-armv7l x86_64-armv7l x86_64-aarch64 x86_64-ppc64le))
        $(error ERROR - cross compiling from $(HOST_ARCH) to $(TARGET_ARCH) is not supported!)
    endif
endif

# operating system
HOST_OS   := $(shell uname -s 2>/dev/null | tr "[:upper:]" "[:lower:]")
TARGET_OS ?= $(HOST_OS)
ifeq (,$(filter $(TARGET_OS),linux darwin qnx android))
    $(error ERROR - unsupported value $(TARGET_OS) for TARGET_OS!)
endif

# host compiler
ifeq ($(TARGET_OS),darwin)
    ifeq ($(shell expr `xcodebuild -version | grep -i xcode | awk '{print $$2}' | cut -d'.' -f1` \>= 5),1)
        HOST_COMPILER ?= clang++
    endif
else ifneq ($(TARGET_ARCH),$(HOST_ARCH))
    ifeq ($(HOST_ARCH)-$(TARGET_ARCH),x86_64-armv7l)
        ifeq ($(TARGET_OS),linux)
            HOST_COMPILER ?= arm-linux-gnueabihf-g++
        else ifeq ($(TARGET_OS),qnx)
            ifeq ($(QNX_HOST),)
                $(error ERROR - QNX_HOST must be passed to the QNX host toolchain)
            endif
            ifeq ($(QNX_TARGET),)
                $(error ERROR - QNX_TARGET must be passed to the QNX target toolchain)
            endif
            export QNX_HOST
            export QNX_TARGET
            HOST_COMPILER ?= $(QNX_HOST)/usr/bin/arm-unknown-nto-qnx6.6.0eabi-g++
        else ifeq ($(TARGET_OS),android)
            HOST_COMPILER ?= arm-linux-androideabi-g++
        endif
    else ifeq ($(TARGET_ARCH),aarch64)
        ifeq ($(TARGET_OS), linux)
            HOST_COMPILER ?= aarch64-linux-gnu-g++
        else ifeq ($(TARGET_OS), android)
            HOST_COMPILER ?= aarch64-linux-android-g++
        endif
    else ifeq ($(TARGET_ARCH),ppc64le)
        HOST_COMPILER ?= powerpc64le-linux-gnu-g++
    endif
endif
HOST_COMPILER ?= g++
NVCC          := $(CUDA_PATH)/bin/nvcc -ccbin $(HOST_COMPILER)

# internal flags
NVCCFLAGS   := -m${TARGET_SIZE}
CCFLAGS     := -I/usr/local/include -stdlib=libstdc++
LDFLAGS     := -L/usr/local/lib

# build flags
ifeq ($(TARGET_OS),darwin)
    LDFLAGS += -rpath $(CUDA_PATH)/lib
    CCFLAGS += -arch $(HOST_ARCH)
else ifeq ($(HOST_ARCH)-$(TARGET_ARCH)-$(TARGET_OS),x86_64-armv7l-linux)
    LDFLAGS += --dynamic-linker=/lib/ld-linux-armhf.so.3
    CCFLAGS += -mfloat-abi=hard
else ifeq ($(TARGET_OS),android)
    LDFLAGS += -pie
    CCFLAGS += -fpie -fpic -fexceptions
endif

ifneq ($(TARGET_ARCH),$(HOST_ARCH))
    ifeq ($(TARGET_ARCH)-$(TARGET_OS),armv7l-linux)
        ifneq ($(TARGET_FS),)
            GCCVERSIONLTEQ46 := $(shell expr `$(HOST_COMPILER) -dumpversion` \<= 4.6)
            ifeq ($(GCCVERSIONLTEQ46),1)
                CCFLAGS += --sysroot=$(TARGET_FS)
            endif
            LDFLAGS += --sysroot=$(TARGET_FS)
            LDFLAGS += -rpath-link=$(TARGET_FS)/lib
            LDFLAGS += -rpath-link=$(TARGET_FS)/usr/lib
            LDFLAGS += -rpath-link=$(TARGET_FS)/usr/lib/arm-linux-gnueabihf
        endif
    endif
endif

# Debug build flags
ifeq ($(dbg),1)
      NVCCFLAGS += -g -G
      BUILD_TYPE := debug
else
      BUILD_TYPE := release
endif

ALL_CCFLAGS :=
ALL_CCFLAGS += $(NVCCFLAGS)
ALL_CCFLAGS += $(EXTRA_NVCCFLAGS)
ALL_CCFLAGS += $(addprefix -Xcompiler ,$(CCFLAGS))
ALL_CCFLAGS += $(addprefix -Xcompiler ,$(EXTRA_CCFLAGS))

ALL_LDFLAGS :=
ALL_LDFLAGS += $(ALL_CCFLAGS)
ALL_LDFLAGS += $(addprefix -Xlinker ,$(LDFLAGS))
ALL_LDFLAGS += $(addprefix -Xlinker ,$(EXTRA_LDFLAGS))

# Common includes and paths for CUDA
INCLUDES  := -I../../common/inc
LIBRARIES := 

################################################################################

UBUNTU = $(shell lsb_release -i -s 2>/dev/null | grep -i ubuntu)

# OpenGL specific libraries
ifeq ($(TARGET_OS),darwin)
 # Mac OSX specific libraries and paths to include
 LIBRARIES +=-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lboost_regex -lopencv_nonfree
else
 LIBRARIES += 
endif

# Gencode arguments
SMS ?=

ifeq ($(GENCODE_FLAGS),)
# Generate SASS code for each SM architecture listed in $(SMS)
$(foreach sm,$(SMS),$(eval GENCODE_FLAGS += -gencode arch=compute_$(sm),code=sm_$(sm)))

ifeq ($(SMS),)
# Generate PTX code from SM 20
GENCODE_FLAGS += -gencode arch=compute_20,code=compute_20
endif

# Generate PTX code from the highest SM architecture in $(SMS) to guarantee forward-compatibility
HIGHEST_SM := $(lastword $(sort $(SMS)))
ifneq ($(HIGHEST_SM),)
GENCODE_FLAGS += -gencode arch=compute_$(HIGHEST_SM),code=compute_$(HIGHEST_SM)
endif
endif

ifeq ($(TARGET_OS),darwin)
  ALL_LDFLAGS += -Xlinker -framework -Xlinker CUDA
else
  CUDA_SEARCH_PATH ?=
  ifneq ($(TARGET_ARCH),$(HOST_ARCH))
    ifeq ($(TARGET_ARCH)-$(TARGET_OS),armv7l-linux)
      ifneq ($(TARGET_FS),)
        LIBRARIES += -L$(TARGET_FS)/usr/lib
        CUDA_SEARCH_PATH += $(TARGET_FS)/usr/lib
        CUDA_SEARCH_PATH += $(TARGET_FS)/usr/lib/arm-linux-gnueabihf
      endif
      CUDA_SEARCH_PATH += $(CUDA_PATH)/targets/armv7-linux-gnueabihf/lib/stubs
      CUDA_SEARCH_PATH += /usr/arm-linux-gnueabihf/lib
    else ifeq ($(TARGET_ARCH)-$(TARGET_OS),armv7l-android)
      CUDA_SEARCH_PATH += $(CUDA_PATH)/targets/armv7-linux-androideabi/lib/stubs
    else ifeq ($(TARGET_ARCH)-$(TARGET_OS),aarch64-android)
      CUDA_SEARCH_PATH += $(CUDA_PATH)/targets/aarch64-linux-androideabi/lib/stubs
    else ifeq ($(TARGET_ARCH)-$(TARGET_OS),ppc64le-linux)
      CUDA_SEARCH_PATH += $(CUDA_PATH)/targets/ppc64le-linux/lib/stubs
    endif
  else
    ifneq ($(UBUNTU),)
      CUDA_SEARCH_PATH += /usr/lib
    else
      CUDA_SEARCH_PATH += /usr/lib64
    endif

    ifeq ($(TARGET_ARCH),x86_64)
      CUDA_SEARCH_PATH += $(CUDA_PATH)/lib64/stubs
    endif

    ifeq ($(TARGET_ARCH),armv7l)
      CUDA_SEARCH_PATH += $(CUDA_PATH)/targets/armv7-linux-gnueabihf/lib/stubs
      CUDA_SEARCH_PATH += /usr/lib/arm-linux-gnueabihf
    endif

    ifeq ($(TARGET_ARCH),aarch64)
      CUDA_SEARCH_PATH += /usr/lib
    endif

    ifeq ($(TARGET_ARCH),ppc64le)
      CUDA_SEARCH_PATH += /usr/lib/powerpc64le-linux-gnu
    endif
  endif

  CUDALIB ?= $(shell find -L $(CUDA_SEARCH_PATH) -maxdepth 1 -name libcuda.so)
  ifeq ("$(CUDALIB)","")
    $(info >>> WARNING - libcuda.so not found, CUDA Driver is not installed.  Please re-install the driver. <<<)
    PROJECT_ENABLED := 0
  endif

  LIBRARIES += -lcuda
endif

LIBRARIES += -lcudart

ifeq ($(PROJECT_ENABLED),0)
EXEC ?= @echo "[@]"
endif

################################################################################

# Target rules
all: build

build: videoNet $(PTX_FILE)

check.deps:
ifeq ($(PROJECT_ENABLED),0)
	@echo "Project will be waived due to the above missing dependencies"
else
	@echo "Project is ready - all dependencies have been met"
endif

$(PTX_FILE): NV12ToARGB_drvapi.cu
	$(EXEC) $(NVCC) $(INCLUDES) $(ALL_CCFLAGS) $(GENCODE_FLAGS) -o $@ -ptx $<
	$(EXEC) mkdir -p data
	$(EXEC) cp -f $@ ./data
	$(EXEC) mkdir -p ./bin/$(TARGET_ARCH)/$(TARGET_OS)/$(BUILD_TYPE)
	$(EXEC) cp -f $@ ./bin/$(TARGET_ARCH)/$(TARGET_OS)/$(BUILD_TYPE)

util/util.o: util/util.cpp
	$(EXEC) $(NVCC) $(INCLUDES) $(ALL_CCFLAGS) $(GENCODE_FLAGS) -o $@ -c $<

MediaReader.o: MediaReader.cpp util/util.o
	$(EXEC) $(NVCC) $(INCLUDES) $(ALL_CCFLAGS) $(GENCODE_FLAGS) -o $@ -c $<

SubtitleReader.o: SubtitleReader.cpp
	$(EXEC) $(NVCC) $(INCLUDES) $(ALL_CCFLAGS) $(GENCODE_FLAGS) -o $@ -c $<

videoNet: SubtitleReader.o MediaReader.o util/util.o
	$(EXEC) $(NVCC) $(ALL_LDFLAGS) $(GENCODE_FLAGS) -o $@ $+ $(LIBRARIES)
	$(EXEC) mkdir -p ./bin/$(TARGET_ARCH)/$(TARGET_OS)/$(BUILD_TYPE)
	$(EXEC) cp $@ ./bin/$(TARGET_ARCH)/$(TARGET_OS)/$(BUILD_TYPE)

run: build
	$(EXEC) ./videoNet

clean:
	rm -f videoNet SubtitleReader.o MediaReader.o  data/$(PTX_FILE) $(PTX_FILE)
	rm -rf ./bin/$(TARGET_ARCH)/$(TARGET_OS)/$(BUILD_TYPE)/videoNet
	rm -rf ./bin/$(TARGET_ARCH)/$(TARGET_OS)/$(BUILD_TYPE)/$(PTX_FILE)

clobber: clean
