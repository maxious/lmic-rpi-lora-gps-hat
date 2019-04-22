function Decoder(b, port) {
var decoded = {};
    switch (port) {
        case 2:
            decoded.lat =  (b[0] + (b[1] << 8) + (b[2] << 16)) / -10000;
            decoded.lon =  (b[3] + (b[4] << 8) + (b[5] << 16)) / -10000;
      decoded.alt = b[6];
      decoded.tdop = b[7]/100;
        break;
        
        case 3:
            decoded.bat = (b[0] | b[1]<<8 | (b[1] & 0x80 ? 0xFF<<16 : 0)) * 10.0;

        break;
    
        case 4:
            decoded.aX = (b[0] | b[1]<<8 | (b[1] & 0x80 ? 0xFF<<16 : 0));
            decoded.aY = (b[2] | b[3]<<8 | (b[3] & 0x80 ? 0xFF<<16 : 0));
            decoded.aZ = (b[4] | b[5]<<8 | (b[5] & 0x80 ? 0xFF<<16 : 0));

        break;
}

return decoded;
}
