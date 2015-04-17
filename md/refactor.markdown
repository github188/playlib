# network

CloudSEE & RTMP tcp connection utils

## interfaces

+ init & deinit
+ connect & disconnect x2
+ query, counts, log, version
+ send bytes, integer, string

## callbacks

+ ConnectChange x2
+ NormalData x2
+ CheckResult
+ ChatData
+ TextData: important
+ Download
+ PlayData
+ LANSData, QueryDevice

## structs

+ `WIFI_INFO`: set ap info. <- app layer
+ `JVRTMP_Metadata_t`: rtmpNormalData callback, -> play, app layer
+ `JVS_FILE_HEADER_EX`: NormalData, PlayData callbacks, -> play, app layer
+ `PAC`, `EXTEND`, `wifiap_t`: TextData callbacks, -> app layer
+ `STLANSRESULT`: LANSData, QueryDevice. -> app layer

# play

full automatically play library.

## interfaces

+ pause & resume
+ drag & scale
+ screenshot(thumb) & record
+ voice, encode, log, version
+ enable/disable audio, playback

## callbacks

only handler, decode, render, encode error

