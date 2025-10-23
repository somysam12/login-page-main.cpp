const form = document.getElementById('loginForm');
const verifyBtn = document.querySelector('.verify-btn');
const statusMessage = document.getElementById('statusMessage');
const licenseKeyInput = document.getElementById('licenseKey');
const rememberKeyCheckbox = document.getElementById('rememberKey');

function showStatus(message, type = 'error') {
    statusMessage.textContent = message;
    statusMessage.className = `status-message ${type}`;
}

function setLoading(isLoading) {
    if (isLoading) {
        verifyBtn.disabled = true;
        verifyBtn.classList.add('loading');
    } else {
        verifyBtn.disabled = false;
        verifyBtn.classList.remove('loading');
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
    try {
        const canvas = document.createElement('canvas');
        const gl = canvas.getContext('webgl');
        if (!gl) {
            return await hashString(navigator.userAgent + screen.width + screen.height);
        }
        const debugInfo = gl.getExtension('WEBGL_debug_renderer_info');
        if (!debugInfo) {
            return await hashString(navigator.userAgent + screen.width + screen.height);
        }
        const renderer = gl.getParameter(debugInfo.UNMASKED_RENDERER_WEBGL);
        const vendor = gl.getParameter(debugInfo.UNMASKED_VENDOR_WEBGL);
        const fingerprint = `${navigator.userAgent}-${renderer}-${vendor}-${screen.width}x${screen.height}`;
        return await hashString(fingerprint);
    } catch (e) {
        return await hashString(navigator.userAgent + screen.width + screen.height);
    }
}

if (localStorage.getItem('rememberKey') === 'true') {
    const savedKey = localStorage.getItem('licenseKey');
    if (savedKey) {
        licenseKeyInput.value = savedKey;
        rememberKeyCheckbox.checked = true;
    }
}

form.addEventListener('submit', async (e) => {
    e.preventDefault();
    
    const licenseKey = licenseKeyInput.value.trim();
    
    if (!licenseKey) {
        showStatus('PLEASE ENTER LICENSE KEY', 'error');
        return;
    }
    
    setLoading(true);
    showStatus('VERIFYING KEY...', 'info');
    
    try {
        const encryptedKey = await hashString(licenseKey);
        const hwid = await getHardwareId();
        
        const response = await fetch('/api/validate', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                username: 'user',
                key: encryptedKey,
                hwid: hwid,
                app_version: '1.0.0'
            })
        });
        
        const data = await response.json();
        
        if (data.success) {
            showStatus('LOGIN SUCCESSFUL!', 'success');
            
            if (rememberKeyCheckbox.checked) {
                localStorage.setItem('rememberKey', 'true');
                localStorage.setItem('licenseKey', licenseKey);
            } else {
                localStorage.removeItem('rememberKey');
                localStorage.removeItem('licenseKey');
            }
            
            setTimeout(() => {
                window.location.href = '/dashboard';
            }, 1500);
        } else {
            showStatus(data.message?.toUpperCase() || 'USER OR GAME NOT REGISTERED', 'error');
        }
    } catch (error) {
        console.error('Login error:', error);
        showStatus('CONNECTION FAILED - CHECK API', 'error');
    } finally {
        setLoading(false);
    }
});

licenseKeyInput.addEventListener('input', () => {
    if (licenseKeyInput.value.trim()) {
        showStatus('READY TO VERIFY', 'info');
    } else {
        showStatus('USER OR GAME NOT REGISTERED', 'error');
    }
});

rememberKeyCheckbox.addEventListener('change', () => {
    if (!rememberKeyCheckbox.checked) {
        localStorage.removeItem('rememberKey');
        localStorage.removeItem('licenseKey');
    }
});
