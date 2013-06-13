require 'sinatra'
require 'rubygems'
require 'ffi'
require 'openssl'
require 'haml'

module FPELib
  extend FFI::Library
  ffi_lib "./fpe.so"

  attach_function :do_fpe, [:string, :string, :pointer], :string
end

set :app_file, __FILE__

get '/' do
  cipher = OpenSSL::Cipher.new('AES-128-CBC')
  cipher.encrypt
  @key = cipher.random_key.unpack('H*')[0]

  haml :index
end

post '/fpe-encrypt' do
  regex = params[:regex]
  msg = params[:msg]
  key = params[:key]
  c = FPELib.do_fpe(regex, msg, key)
  c
end