Its a simple demo to test wifi and internet connectivity on maixduino board using kendryte-standalone-sdk.



It seems that esp32 can't read fast enough data from website to fill buffer. 
From some trials i found that socket read buffer length 128 is small enough to let esp32 keep it filled.
With bigger buffer some delay is required between consecutive reads.
