# Backend API Implementation Example

This document provides example implementations for the backend API endpoints required by the BR MODS EXTERNAL login system.

## Database Schema

### Users Table
```sql
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(255) UNIQUE NOT NULL,
    license_key VARCHAR(64) NOT NULL,
    hwid VARCHAR(64),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    expires_at TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE
);
```

### Sessions Table
```sql
CREATE TABLE sessions (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(255) NOT NULL,
    session_token VARCHAR(64) UNIQUE NOT NULL,
    hwid VARCHAR(64) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_activity TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    expires_at TIMESTAMP,
    is_active BOOLEAN DEFAULT TRUE,
    FOREIGN KEY (username) REFERENCES users(username)
);
```

## Node.js/Express Example

```javascript
const express = require('express');
const crypto = require('crypto');
const app = express();

app.use(express.json());

// Middleware for CORS (adjust for your domain)
app.use((req, res, next) => {
    res.header('Access-Control-Allow-Origin', '*');
    res.header('Access-Control-Allow-Headers', 'Content-Type');
    next();
});

// POST /api/validate
app.post('/api/validate', async (req, res) => {
    const { username, key, hwid, app_version } = req.body;
    
    // Validate input
    if (!username || !key || !hwid) {
        return res.json({
            success: false,
            message: 'Missing required fields'
        });
    }
    
    // Check if user exists and key matches
    const user = await db.query(
        'SELECT * FROM users WHERE username = ? AND license_key = ? AND is_active = TRUE',
        [username, key]
    );
    
    if (!user || user.length === 0) {
        return res.json({
            success: false,
            message: 'Invalid credentials'
        });
    }
    
    // Check if key has expired
    if (user[0].expires_at && new Date(user[0].expires_at) < new Date()) {
        return res.json({
            success: false,
            message: 'License key has expired'
        });
    }
    
    // Check for existing active session
    const existingSession = await db.query(
        'SELECT * FROM sessions WHERE username = ? AND is_active = TRUE',
        [username]
    );
    
    if (existingSession && existingSession.length > 0) {
        // Check if it's the same hardware
        if (existingSession[0].hwid !== hwid) {
            return res.json({
                success: false,
                message: 'User already in use!'
            });
        }
        
        // Same hardware, update existing session
        await db.query(
            'UPDATE sessions SET last_activity = NOW() WHERE id = ?',
            [existingSession[0].id]
        );
        
        return res.json({
            success: true,
            session_token: existingSession[0].session_token,
            expires_at: Math.floor(new Date(existingSession[0].expires_at).getTime() / 1000),
            message: 'Login successful'
        });
    }
    
    // Create new session
    const sessionToken = crypto.randomBytes(32).toString('hex');
    const expiresAt = new Date(Date.now() + 24 * 60 * 60 * 1000); // 24 hours
    
    await db.query(
        'INSERT INTO sessions (username, session_token, hwid, expires_at) VALUES (?, ?, ?, ?)',
        [username, sessionToken, hwid, expiresAt]
    );
    
    // Update user's HWID
    await db.query(
        'UPDATE users SET hwid = ? WHERE username = ?',
        [hwid, username]
    );
    
    res.json({
        success: true,
        session_token: sessionToken,
        expires_at: Math.floor(expiresAt.getTime() / 1000),
        message: 'Login successful'
    });
});

// POST /api/check-session
app.post('/api/check-session', async (req, res) => {
    const { session_token, username } = req.body;
    
    if (!session_token || !username) {
        return res.json({ valid: false });
    }
    
    const session = await db.query(
        'SELECT * FROM sessions WHERE session_token = ? AND username = ? AND is_active = TRUE',
        [session_token, username]
    );
    
    if (!session || session.length === 0) {
        return res.json({ valid: false });
    }
    
    // Check if session has expired
    if (new Date(session[0].expires_at) < new Date()) {
        await db.query(
            'UPDATE sessions SET is_active = FALSE WHERE id = ?',
            [session[0].id]
        );
        return res.json({ valid: false });
    }
    
    // Update last activity
    await db.query(
        'UPDATE sessions SET last_activity = NOW() WHERE id = ?',
        [session[0].id]
    );
    
    res.json({ valid: true });
});

// POST /api/logout
app.post('/api/logout', async (req, res) => {
    const { session_token, username } = req.body;
    
    if (!session_token || !username) {
        return res.json({ success: false });
    }
    
    await db.query(
        'UPDATE sessions SET is_active = FALSE WHERE session_token = ? AND username = ?',
        [session_token, username]
    );
    
    res.json({ success: true });
});

app.listen(3000, () => {
    console.log('API server running on port 3000');
});
```

## PHP Example

```php
<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Headers: Content-Type');

// Database connection
$db = new PDO('mysql:host=localhost;dbname=your_db', 'username', 'password');

// POST /api/validate
if ($_SERVER['REQUEST_METHOD'] === 'POST' && $_SERVER['REQUEST_URI'] === '/api/validate') {
    $data = json_decode(file_get_contents('php://input'), true);
    
    $username = $data['username'] ?? '';
    $key = $data['key'] ?? '';
    $hwid = $data['hwid'] ?? '';
    
    if (empty($username) || empty($key) || empty($hwid)) {
        echo json_encode(['success' => false, 'message' => 'Missing required fields']);
        exit;
    }
    
    // Check user credentials
    $stmt = $db->prepare('SELECT * FROM users WHERE username = ? AND license_key = ? AND is_active = 1');
    $stmt->execute([$username, $key]);
    $user = $stmt->fetch(PDO::FETCH_ASSOC);
    
    if (!$user) {
        echo json_encode(['success' => false, 'message' => 'Invalid credentials']);
        exit;
    }
    
    // Check expiration
    if ($user['expires_at'] && strtotime($user['expires_at']) < time()) {
        echo json_encode(['success' => false, 'message' => 'License key has expired']);
        exit;
    }
    
    // Check existing session
    $stmt = $db->prepare('SELECT * FROM sessions WHERE username = ? AND is_active = 1');
    $stmt->execute([$username]);
    $session = $stmt->fetch(PDO::FETCH_ASSOC);
    
    if ($session && $session['hwid'] !== $hwid) {
        echo json_encode(['success' => false, 'message' => 'User already in use!']);
        exit;
    }
    
    // Create or update session
    $sessionToken = bin2hex(random_bytes(32));
    $expiresAt = date('Y-m-d H:i:s', time() + 86400);
    
    if ($session) {
        $stmt = $db->prepare('UPDATE sessions SET last_activity = NOW() WHERE id = ?');
        $stmt->execute([$session['id']]);
        $sessionToken = $session['session_token'];
    } else {
        $stmt = $db->prepare('INSERT INTO sessions (username, session_token, hwid, expires_at) VALUES (?, ?, ?, ?)');
        $stmt->execute([$username, $sessionToken, $hwid, $expiresAt]);
    }
    
    echo json_encode([
        'success' => true,
        'session_token' => $sessionToken,
        'expires_at' => strtotime($expiresAt),
        'message' => 'Login successful'
    ]);
}

// POST /api/check-session
if ($_SERVER['REQUEST_METHOD'] === 'POST' && $_SERVER['REQUEST_URI'] === '/api/check-session') {
    $data = json_decode(file_get_contents('php://input'), true);
    
    $sessionToken = $data['session_token'] ?? '';
    $username = $data['username'] ?? '';
    
    $stmt = $db->prepare('SELECT * FROM sessions WHERE session_token = ? AND username = ? AND is_active = 1');
    $stmt->execute([$sessionToken, $username]);
    $session = $stmt->fetch(PDO::FETCH_ASSOC);
    
    if ($session && strtotime($session['expires_at']) > time()) {
        $stmt = $db->prepare('UPDATE sessions SET last_activity = NOW() WHERE id = ?');
        $stmt->execute([$session['id']]);
        echo json_encode(['valid' => true]);
    } else {
        echo json_encode(['valid' => false]);
    }
}

// POST /api/logout
if ($_SERVER['REQUEST_METHOD'] === 'POST' && $_SERVER['REQUEST_URI'] === '/api/logout') {
    $data = json_decode(file_get_contents('php://input'), true);
    
    $sessionToken = $data['session_token'] ?? '';
    $username = $data['username'] ?? '';
    
    $stmt = $db->prepare('UPDATE sessions SET is_active = 0 WHERE session_token = ? AND username = ?');
    $stmt->execute([$sessionToken, $username]);
    
    echo json_encode(['success' => true]);
}
?>
```

## Python/Flask Example

```python
from flask import Flask, request, jsonify
from flask_cors import CORS
import hashlib
import secrets
from datetime import datetime, timedelta
import mysql.connector

app = Flask(__name__)
CORS(app)

db = mysql.connector.connect(
    host="localhost",
    user="username",
    password="password",
    database="your_db"
)

@app.route('/api/validate', methods=['POST'])
def validate():
    data = request.get_json()
    
    username = data.get('username')
    key = data.get('key')
    hwid = data.get('hwid')
    
    if not all([username, key, hwid]):
        return jsonify({'success': False, 'message': 'Missing required fields'})
    
    cursor = db.cursor(dictionary=True)
    
    # Check credentials
    cursor.execute(
        'SELECT * FROM users WHERE username = %s AND license_key = %s AND is_active = TRUE',
        (username, key)
    )
    user = cursor.fetchone()
    
    if not user:
        return jsonify({'success': False, 'message': 'Invalid credentials'})
    
    # Check expiration
    if user['expires_at'] and user['expires_at'] < datetime.now():
        return jsonify({'success': False, 'message': 'License key has expired'})
    
    # Check existing session
    cursor.execute(
        'SELECT * FROM sessions WHERE username = %s AND is_active = TRUE',
        (username,)
    )
    session = cursor.fetchone()
    
    if session and session['hwid'] != hwid:
        return jsonify({'success': False, 'message': 'User already in use!'})
    
    # Create or update session
    if session:
        cursor.execute(
            'UPDATE sessions SET last_activity = NOW() WHERE id = %s',
            (session['id'],)
        )
        db.commit()
        session_token = session['session_token']
        expires_at = session['expires_at']
    else:
        session_token = secrets.token_hex(32)
        expires_at = datetime.now() + timedelta(days=1)
        
        cursor.execute(
            'INSERT INTO sessions (username, session_token, hwid, expires_at) VALUES (%s, %s, %s, %s)',
            (username, session_token, hwid, expires_at)
        )
        db.commit()
    
    return jsonify({
        'success': True,
        'session_token': session_token,
        'expires_at': int(expires_at.timestamp()),
        'message': 'Login successful'
    })

@app.route('/api/check-session', methods=['POST'])
def check_session():
    data = request.get_json()
    
    session_token = data.get('session_token')
    username = data.get('username')
    
    cursor = db.cursor(dictionary=True)
    cursor.execute(
        'SELECT * FROM sessions WHERE session_token = %s AND username = %s AND is_active = TRUE',
        (session_token, username)
    )
    session = cursor.fetchone()
    
    if session and session['expires_at'] > datetime.now():
        cursor.execute(
            'UPDATE sessions SET last_activity = NOW() WHERE id = %s',
            (session['id'],)
        )
        db.commit()
        return jsonify({'valid': True})
    
    return jsonify({'valid': False})

@app.route('/api/logout', methods=['POST'])
def logout():
    data = request.get_json()
    
    session_token = data.get('session_token')
    username = data.get('username')
    
    cursor = db.cursor()
    cursor.execute(
        'UPDATE sessions SET is_active = FALSE WHERE session_token = %s AND username = %s',
        (session_token, username)
    )
    db.commit()
    
    return jsonify({'success': True})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=3000, ssl_context='adhoc')
```

## Security Best Practices

1. **Use HTTPS**: Always use SSL/TLS certificates
2. **Hash Keys**: Store license keys hashed in database
3. **Rate Limiting**: Implement rate limiting on login attempts
4. **Input Validation**: Validate and sanitize all inputs
5. **Session Expiry**: Implement automatic session cleanup
6. **IP Logging**: Log IP addresses for security monitoring
7. **HWID Validation**: Verify hardware ID consistency

## Testing

Use tools like Postman or curl to test your endpoints:

```bash
curl -X POST https://your-website.com/api/validate \
  -H "Content-Type: application/json" \
  -d '{
    "username": "testuser",
    "key": "hashed_key_here",
    "hwid": "hardware_id_hash",
    "app_version": "1.0.0"
  }'
```
