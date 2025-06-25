import os
import soundfile as sf
import launch_usat

def main():
    # Find a .wav and .xml file in the current directory
    wav_files = [f for f in os.listdir('.') if f.lower().endswith('.wav')]
    xml_files = [f for f in os.listdir('.') if f.lower().endswith('.xml')]

    if not wav_files:
        print("No WAV file found in the current directory.")
        return
    if not xml_files:
        print("No XML file found in the current directory.")
        return

    audio_path = wav_files[0]
    xml_path = xml_files[0]

    print(f"Loading audio: {audio_path}")
    audio_data, samplerate = sf.read(audio_path)  # audio_data is a NumPy array

    print(f"Loading XML config: {xml_path}")

    # Call the launch_usat function
    result      = launch_usat.start_decoding(xml_path)
    gain_matrix = result[0].T

    

if __name__ == "__main__":
    main()
