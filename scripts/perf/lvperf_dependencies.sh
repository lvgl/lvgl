#!/bin/sh

# This script installs extra dependencies for the lvperf Docker images used in the CI pipeline.
# The images are based on Alpine Linux and support runtime installation of dependencies,
# allowing you to extend functionality without rebuilding the image.
#
# For guidance on how dependencies are typically added, refer to the Dockerfiles:
# - https://github.com/smartobjectoriented/so3/blob/main/docker/Dockerfile.lvperf_32b
# - https://github.com/smartobjectoriented/so3/blob/main/docker/Dockerfile.lvperf_64b
