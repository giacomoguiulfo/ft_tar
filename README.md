# ft_tar

ft_tar is a rush project which means that it is limited to 2 days in which time we had to recreate the functionality of the `tar` command.

Our program has to conform to `Norm` which introduces numerous restrictions such as the inability to use `for` loops, 25 lines per function, etc. 

### Building

>```
>$ git submodule init
>$ git submodule update
>$ make

### Running
```
ft_tar - manipulate tape archives

-c		Create a new archive containing the specified items.
-f=file		Read/write the archive from/to the specified file.
-p		(x mode only) Preserve file permissions.
-t		List archive contents to stdout.
-v		Produce verbose output.
-x		Extract to disk from the archive.
-z		Use GZIP(1) compression.
--help		display this help and exit
```

![No manual entry for ft_tar](https://imgs.xkcd.com/comics/tar_2x.png)