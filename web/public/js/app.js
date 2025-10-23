const form = document.getElementById('loginForm');
const loginBtn = document.getElementById('loginBtn');
const btnText = document.getElementById('btnText');
const messageBox = document.getElementById('messageBox');
const usernameInput = document.getElementById('username');
const licenseKeyInput = document.getElementById('licenseKey');

function showMessage(message, type = 'info') {
    messageBox.textContent = message;
    messageBox.className = `message-box show ${type}`;
    
    setTimeout(() => {
        messageBox.classList.remove('show');
    }, 5000);
}

function setLoading(isLoading) {
    if (isLoading) {
        loginBtn.disabled = true;
        loginBtn.classList.add('loading');
        btnText.textContent = 'AUTHENTICATING...';
    } else {
        loginBtn.disabled = false;
        loginBtn.classList.remove('loading');
        btnText.textContent = 'LOGIN';
    }
}

async function hashString(str) {
    const encoder = new TextEncoder();
    const data = encoder.encode(str);
    const hashBuffer = await crypto.subtle.digest('SHA-256', data);
    const hashArray = Array.from(new Uint8Array(hashBuffer));
    return hashArray.map(b => b.toString(16).padStart(2, '0')).join('');
}

async function getHardwareId() {
    const canvas = document.createElement('canvas');
    const gl = canvas.getContext('webgl');
    const debugInfo = gl.getExtension('WEBGL_debug_renderer_info');
    const renderer = gl.getParameter(debugInfo.UNMASKED_RENDERER_WEBGL);
    const vendor = gl.getParameter(debugInfo.UNMASKED_VENDOR_WEBGL);
    
    const fingerprint = `${navigator.userAgent}-${renderer}-${vendor}-${screen.width}x${screen.height}`;
    return await hashString(fingerprint);
}

form.addEventListener('submit', async (e) => {
    e.preventDefault();
    
    const username = usernameInput.value.trim();
    const licenseKey = licenseKeyInput.value.trim();
    
    if (!username || !licenseKey) {
        showMessage('Please fill in all fields', 'error');
        return;
    }
    
    setLoading(true);
    messageBox.classList.remove('show');
    
    try {
        const encryptedKey = await hashString(licenseKey);
        const hwid = await getHardwareId();
        
        const response = await fetch('/api/validate', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                username: username,
                key: encryptedKey,
                hwid: hwid,
                app_version: '1.0.0'
            })
        });
        
        const data = await response.json();
        
        if (data.success) {
            showMessage(data.message || 'Login successful!', 'success');
            
            setTimeout(() => {
                window.location.href = '/dashboard';
            }, 1500);
        } else {
            showMessage(data.message || 'Login failed', 'error');
        }
    } catch (error) {
        console.error('Login error:', error);
        showMessage('Unable to connect to server. Please check your API configuration.', 'error');
    } finally {
        setLoading(false);
    }
});

usernameInput.addEventListener('input', () => {
    messageBox.classList.remove('show');
});

licenseKeyInput.addEventListener('input', () => {
    messageBox.classList.remove('show');
});
