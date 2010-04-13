# -*- coding: utf-8 -
#
# This file is part of restkit released under the MIT license. 
# See the NOTICE for more information.

import os

try:
    from cStringIO import StringIO
except ImportError:
    from StringIO import StringIO

import tempfile

class TeeInput(object):
    
    def __init__(self, message, buf):
        self.buf = StringIO()
        self.message = message
        
        if not self.parser.content_len and not self.parser.is_chunked:
            self.tmp = buf
            self._len = len(buf.getvalue())
            self._is_socket = False
        elif self._len and self._len < sock.MAX_BODY:
            self.tmp = StringIO()
        else:
            self.tmp = tempfile.TemporaryFile()
        
        if len(buf.getvalue()) > 0:
            chunk, self.buf = parser.filter_body(buf)
            if chunk:
                self.tmp.write(chunk)
                self.tmp.flush()
            self._finalize()
            self.tmp.seek(0)
            del buf
                    
    @property
    def len(self):
        if self._len: return self._len
        
        if self._is_socket:
            pos = self.tmp.tell()
            self.tmp.seek(0, 2)
            while True:
                if not self._tee(self.CHUNK_SIZE):
                    break
            self.tmp.seek(pos)
        self._len = self._tmp_size()
        return self._len
        
    def seek(self, offset, whence=0):
        """ naive implementation of seek """
        if self._is_socket:
            self.tmp.seek(0, 2)
            while True:
                if not self._tee(self.CHUNK_SIZE):
                    break
        self.tmp.seek(offset, whence)

    def flush(self):
        self.tmp.flush()
        
    def read(self, length=-1):
        """ read """
        if not self._is_socket:
            return self.tmp.read(length)
            
        if length < 0:
            buf = StringIO()
            buf.write(self.tmp.read())
            while True:
                chunk = self._tee(self.CHUNK_SIZE)
                if not chunk: 
                    break
                buf.write(chunk)
            return buf.getvalue()
        else:
            dest = StringIO()
            diff = self._tmp_size() - self.tmp.tell()
            if not diff:
                dest.write(self._tee(length))
                return self._ensure_length(dest, length)
            else:
                l = min(diff, length)
                dest.write(self.tmp.read(l))
                return self._ensure_length(dest, length)
                
    def readline(self, size=-1):
        if not self._is_socket:
            return self.tmp.readline()
        
        orig_size = self._tmp_size()
        if self.tmp.tell() == orig_size:
            if not self._tee(self.CHUNK_SIZE):
                return ''
            self.tmp.seek(orig_size)
        
        # now we can get line
        line = self.tmp.readline()
        if line.find("\n") >=0:
            return line

        buf = StringIO()
        buf.write(line)
        while True:
            orig_size = self.tmp.tell()
            data = self._tee(self.CHUNK_SIZE)
            if not data:
                break
            self.tmp.seek(orig_size)
            buf.write(self.tmp.readline())
            if data.find("\n") >= 0:
                break
        return buf.getvalue()
       
    def readlines(self, sizehint=0):
        total = 0
        lines = []
        line = self.readline()
        while line:
            lines.append(line)
            total += len(line)
            if 0 < sizehint <= total:
                break
            line = self.readline()
        return lines
    
    def close(self):
        if callable(self.maybe_close):
            self.maybe_close()
        
        self.buf = StringIO()
        self._is_socket = False
    
    def next(self):
        r = self.readline()
        if not r:
            raise StopIteration
        return r
    __next__ = next
    
    def __iter__(self):
        return self    

    def _tee(self, length):
        """ fetch partial body"""
        buf2 = self.buf
        buf2.seek(0, 2) 
        while True:
            chunk, buf2 = self.parser.filter_body(buf2)
            if chunk:
                self.tmp.write(chunk)
                self.tmp.flush()
                self.tmp.seek(0, 2)
                self.buf = StringIO()
                self.buf.write(buf2.getvalue())
                return chunk

            if self.parser.body_eof():
                break
                
            if not self._is_socket:
                if self.parser.is_chunked:
                    data = buf2.getvalue()
                    if data.find("\r\n") >= 0:
                        continue
                raise UnexpectedEOF("remote closed the connection")

            data = self._sock.recv(length)
            if not data:
                self._is_socket = False
            buf2.write(data)
        
        self._finalize()
        return ""
        
    def _finalize(self):
        """ here we wil fetch final trailers
        if any."""

        if self.parser.body_eof():
            self.close()

    def _tmp_size(self):
        if hasattr(self.tmp, 'fileno'):
            return int(os.fstat(self.tmp.fileno())[6])
        else:
            return len(self.tmp.getvalue())
            
    def _ensure_length(self, dest, length):
        if not len(dest.getvalue()) or not self._len:
            return dest.getvalue()
        while True:
            if len(dest.getvalue()) >= length: 
                break
            data = self._tee(length - len(dest.getvalue()))
            if not data: 
                break
            dest.write(data)
        return dest.getvalue()