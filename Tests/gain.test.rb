#!/usr/bin/env ruby -wKU
# encoding: utf-8

require 'Jamoma'

environment = TTObject.new "environment"
environment.set "benchmarking", 1

o = TTObject.new "gain"
o.send "test"

err, cpu = o.send "getProcessingBenchmark", 1

puts
puts "time spent calculating audio process method: #{cpu} µs"
puts
