# X-Star-Wars: An X-Wing simulation

## Download
```
git clone --recurse-submodules https://github.com/moleculeATP/x-star_wars.git
```

or 

```
git clone https://github.com/moleculeATP/x-star_wars.git
cd x-star_wars/
git submodule init
git submodule update
```

## Building
```
cmake -B build/
cd ./build
make -j {num_proc}
```

## Running
Once built:
```
./build/.x-star-wars
```

## Credits
- Damien Rohmer (https://github.com/drohmer/CGP)