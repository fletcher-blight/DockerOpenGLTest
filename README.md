```commandline
# Build Container
docker build -t gltest .

# Mount Code In Container to Configure and Build
docker run -v `pwd`:/tmp/gl/ gltest cmake -S /tmp/gl/ -B /tmp/gl/build
# Then Build
docker run -v `pwd`:/tmp/gl/ gltest cmake --build /tmp/gl/build

# Run Inside Container Forwarding X11 Display 
docker run -v `pwd`:/tmp/gl/  -v /tmp/.X11-unix/:/tmp/.X11-unix/ -e DISPLAY=$DISPLAY gltest /tmp/gl/build/gl
```