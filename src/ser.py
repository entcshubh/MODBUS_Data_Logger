from flask import Flask, request, jsonify

app = Flask(__name__)

# Root endpoint to receive ESP32 data
@app.route('/', methods=['POST'])
def receive_data():
    try:
        print("\n📥 ESP32 HTTP POST received")

        # Raw payload
        raw_data = request.data
        print("📦 Raw Data:", raw_data)

        # Parse JSON
        json_data = request.get_json(silent=True)

        if json_data:
            print("🟢 Parsed JSON:", json_data)
        else:
            print("⚠️ Payload is not JSON")

        return jsonify({
            "status": "OK",
            "message": "Data received"
        }), 200

    except Exception as e:
        print("❌ Error:", e)
        return jsonify({
            "status": "error",
            "message": str(e)
        }), 400


if __name__ == '__main__':
    print("\n🚀 HTTP Server started")
    print("Listening on port 7778...\n")

    app.run(host='0.0.0.0', port=7778, debug=True)