// SoundEngine.swift — Sine-wave tone generator using AVAudioEngine.
// Converts the OCR2A register-value notes used by the hardware into
// audio frequencies: f = 1,000,000 / (note + 1) Hz.
// © 2026 28pins — https://github.com/28pins/TWSUGamerPlus
// SPDX-License-Identifier: MIT

import AVFoundation

final class SoundEngine {

    private let engine   = AVAudioEngine()
    private let player   = AVAudioPlayerNode()
    private let mixer    = AVAudioMixerNode()

    private let sampleRate: Double = 44100
    private let bufferSize: AVAudioFrameCount = 4096

    private var isSetUp = false
    private var currentNote: Int? = nil

    init() {
        setup()
    }

    // MARK: - Public API

    func playTone(note: Int) {
        guard isSetUp, note != currentNote else { return }
        currentNote = note
        let frequency = 1_000_000.0 / Double(note + 1)
        scheduleBuffer(frequency: frequency, looping: true)
        if !player.isPlaying { player.play() }
    }

    func stopTone() {
        guard isSetUp else { return }
        currentNote = nil
        player.stop()
    }

    // MARK: - Private

    private func setup() {
        do {
            let session = AVAudioSession.sharedInstance()
            try session.setCategory(.playback, options: .mixWithOthers)
            try session.setActive(true)
        } catch {
            // Silent failure – sound is optional in emulator
            return
        }

        engine.attach(player)
        engine.attach(mixer)

        let format = AVAudioFormat(standardFormatWithSampleRate: sampleRate, channels: 1)!
        engine.connect(player, to: mixer, format: format)
        engine.connect(mixer, to: engine.mainMixerNode, format: format)
        mixer.outputVolume = 0.25

        do {
            try engine.start()
            isSetUp = true
        } catch {
            isSetUp = false
        }
    }

    private func scheduleBuffer(frequency: Double, looping: Bool) {
        let format = AVAudioFormat(standardFormatWithSampleRate: sampleRate, channels: 1)!
        guard let buffer = AVAudioPCMBuffer(pcmFormat: format, frameCapacity: bufferSize) else { return }
        buffer.frameLength = bufferSize

        let channelData = buffer.floatChannelData![0]
        let angularFrequency = 2.0 * Double.pi * frequency / sampleRate

        for frame in 0..<Int(bufferSize) {
            channelData[frame] = Float(sin(angularFrequency * Double(frame)))
        }

        player.stop()
        let options: AVAudioPlayerNodeBufferOptions = looping ? [.loops] : []
        player.scheduleBuffer(buffer, at: nil, options: options, completionHandler: nil)
    }
}
