const express = require('express');
const path = require('path');
const app = express();

app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

app.post('/api/validate', async (req, res) => {
    const { username, key, hwid, app_version } = req.body;
    
    console.log('Login attempt:', { username, hwid, app_version });
    
    res.json({
        success: false,
        message: 'Demo mode: Please configure your API endpoint in the backend'
    });
});

app.get('/dashboard', (req, res) => {
    res.send(`
        <!DOCTYPE html>
        <html>
        <head>
            <title>Dashboard</title>
            <style>
                body {
                    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
                    background: linear-gradient(135deg, #0a0a0f 0%, #14141f 100%);
                    color: white;
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    min-height: 100vh;
                    margin: 0;
                }
                .container {
                    text-align: center;
                    padding: 40px;
                    background: rgba(18, 18, 24, 0.95);
                    border-radius: 16px;
                    border: 1px solid rgba(255, 255, 255, 0.1);
                }
                h1 {
                    background: linear-gradient(135deg, #66ccff 0%, #4d99ff 100%);
                    -webkit-background-clip: text;
                    -webkit-text-fill-color: transparent;
                    background-clip: text;
                    margin-bottom: 20px;
                }
                a {
                    color: #66ccff;
                    text-decoration: none;
                }
            </style>
        </head>
        <body>
            <div class="container">
                <h1>✅ Login Successful!</h1>
                <p>Welcome to the dashboard</p>
                <p style="margin-top: 20px;"><a href="/">← Back to Login</a></p>
            </div>
        </body>
        </html>
    `);
});

const PORT = process.env.PORT || 5000;
app.listen(PORT, '0.0.0.0', () => {
    console.log(`🚀 Login System running on port ${PORT}`);
    console.log(`📱 Web UI: http://localhost:${PORT}`);
});
