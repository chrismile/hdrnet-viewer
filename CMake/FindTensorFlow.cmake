find_library(TF_LIBRARIES NAMES tensorflow_cc PATHS "/usr/local/lib/tensorflow_cc/")
include_directories("/usr/local/include/tensorflow/")
include_directories("/usr/local/include/tensorflow/bazel-genfiles")
include_directories("/usr/local/include/tensorflow/tensorflow/contrib/makefile/downloads")
include_directories("/usr/local/include/tensorflow/tensorflow/contrib/makefile/downloads/eigen")
include_directories("/usr/local/include/tensorflow/tensorflow/contrib/makefile/downloads/gemmlowp")
include_directories("/usr/local/include/tensorflow/tensorflow/contrib/makefile/gen/protobuf-host/include")
set(TF_INCLUDE_DIR "/usr/local/include/tensorflow/")
link_directories("/usr/local/lib/tensorflow_cc/")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TensorFlow DEFAULT_MSG TF_LIBRARIES TF_INCLUDE_DIR)

mark_as_advanced(TF_INCLUDE_DIR TF_LIBRARIES)
