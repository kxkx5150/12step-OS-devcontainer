# 12ステップで作る 組込みOS自作入門 devcontainer

<br>

## [H8/3069F](https://akizukidenshi.com/catalog/g/gK-01271/) emulator included  

[original h8_emulator](https://github.com/shimomura1004/h8_emulator)

<br><br><br><br>

## Build

<br>

#### Emulator  

F7 key

<br><br>

#### OS

<br>

```
cd src/12step_os/12
```

##### build bootload
```
make boot
```

##### build os
```
make os
```

##### bild bootload + os
```
make
```

##### clean
```
make clean
```

<br><br><br>
<br><br><br>


## Usage

<br>

#### cd exe directory
```
./h8emu kzload.elf
```

#### split terminal (ctrl + shift + 5)
```
./sender ser1
```

<br>

kzload>
```
load
```
press ":" key  
(command)
```
send kozos
```
kzload>
```
run
```




<br><br><br>

quit  
press ctrl + c

<br><br><br>

F5  
debug emulator

<br><br><br>
<br><br><br>
<br><br><br>
