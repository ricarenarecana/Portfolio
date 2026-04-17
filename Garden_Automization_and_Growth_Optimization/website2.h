const char websiteHtml[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Gardening Automation and Growth Optimization</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }

        body {
            font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(to bottom, white, #e6f7e9);
            color: #333333;
            min-height: 100vh;
        }

        .container {
            max-width: 1200px;
            margin: 0 auto;
            padding: 32px 16px;
        }

        .garden-title {
            font-size: 28px;
            font-weight: bold;
            text-align: center;
            margin-bottom: 32px;
            color: #16a34a;
        }

        .garden-subtitle {
            font-size: 20px;
            font-weight: 500;
            text-align: center;
            margin-bottom: 24px;
            color: #4b5563;
        }

        .dashboard-section {
            margin-bottom: 40px;
        }

        .section-title {
            font-size: 24px;
            font-weight: 600;
            margin-bottom: 24px;
            color: #16a34a;
            border-bottom: 2px solid #e5e7eb;
            padding-bottom: 8px;
        }

        .garden-grid {
            display: grid;
            grid-template-columns: repeat(1, 1fr);
            gap: 24px;
        }

        @media (min-width: 768px) {
            .garden-grid {
                grid-template-columns: repeat(2, 1fr);
            }
        }

        @media (min-width: 1024px) {
            .garden-grid {
                grid-template-columns: repeat(4, 1fr);
            }
        }

        .garden-card {
            background-color: white;
            border-radius: 8px;
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
            overflow: hidden;
        }

        .garden-card-header {
            padding: 16px 16px 8px 16px;
            border-bottom: 1px solid #f0f0f0;
        }

        .garden-card-title {
            font-size: 18px;
            font-weight: 600;
        }

        .garden-card-content {
            padding: 16px;
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 16px;
        }

        .status-icon {
            height: 64px;
            width: 64px;
            stroke-width: 1.5;
        }

        .status-icon.active.light-icon {
            color: #fcd34d;
            animation: pulse 2s infinite;
        }

        .status-icon.active.water-icon {
            color: #0ea5e9;
            animation: pulse 2s infinite;
        }

        .status-icon.inactive {
            color: #d1d5db;
        }

        .status-toggle {
            display: flex;
            align-items: center;
            gap: 8px;
        }

        .status-label {
            font-size: 14px;
            font-weight: 500;
        }

        .switch {
            position: relative;
            display: inline-block;
            width: 44px;
            height: 24px;
        }

        .switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }

        .slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background-color: #cccccc;
            transition: 0.4s;
            border-radius: 24px;
        }

        .slider:before {
            position: absolute;
            content: "";
            height: 20px;
            width: 20px;
            left: 2px;
            bottom: 2px;
            background-color: white;
            transition: 0.4s;
            border-radius: 50%;
        }

        input:checked+.slider {
            background-color: #16a34a;
        }

        input:checked+.slider.light {
            background-color: #d97706;
        }

        input:checked+.slider.water {
            background-color: #0369a1;
        }

        input:checked+.slider:before {
            transform: translateX(20px);
        }

        .moisture-gauge {
            width: 100%;
        }

        .moisture-sensor-label {
            font-size: 14px;
            font-weight: 600;
            color: #4b5563;
            margin-bottom: 8px;
            align-self: flex-start;
        }

        .moisture-sensor-divider {
            width: 100%;
            height: 1px;
            background-color: #e5e7eb;
            margin: 16px 0;
        }

        .moisture-labels {
            display: flex;
            justify-content: space-between;
            margin-bottom: 4px;
            font-size: 14px;
        }

        .progress-container {
            width: 100%;
            background-color: #e5e7eb;
            border-radius: 4px;
            height: 12px;
            margin-bottom: 8px;
            overflow: hidden;
        }

        .progress-bar {
            height: 100%;
            border-radius: 4px;
            transition: width 0.3s ease;
        }

        .progress-bar.dry {
            background-color: #ef4444;
        }

        .progress-bar.moist {
            background-color: #8b5cf6;
        }

        .progress-bar.wet {
            background-color: #0ea5e9;
        }

        .progress-bar.dark {
            background-color: #4b5563;
        }

        .progress-bar.dim {
            background-color: #9ca3af;
        }

        .progress-bar.bright {
            background-color: #fbbf24;
        }

        .moisture-value {
            display: flex;
            align-items: center;
            gap: 8px;
        }

        .moisture-percent {
            font-weight: 500;
        }

        .moisture-status {
            font-size: 14px;
            color: #6b7280;
        }

        .plant-normal {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 8px;
            text-align: center;
        }

        .plant-normal .leaf-icon {
            height: 64px;
            width: 64px;
            stroke-width: 1.5;
            color: #4ade80;
        }

        .plant-normal .status-text {
            font-size: 14px;
            color: #6b7280;
        }

        .plant-alert {
            background-color: rgba(74, 222, 128, 0.1);
            border: 1px solid #4ade80;
            border-radius: 8px;
            padding: 12px 16px;
            display: flex;
            flex-direction: column;
            gap: 4px;
        }

        .plant-alert-header {
            display: flex;
            align-items: center;
            gap: 8px;
        }

        .plant-alert .leaf-icon {
            height: 64px;
            width: 64px;
            color: #4ade80;
        }

        .plant-alert-title {
            font-weight: 600;
            color: #16a34a;
        }

        .plant-alert-desc {
            font-size: 14px;
            color: #374151;
        }

        .sms-form {
            width: 100%;
            display: flex;
            flex-direction: column;
            gap: 16px;
        }

        .input-group {
            width: 100%;
            display: flex;
            flex-direction: column;
            gap: 4px;
        }

        .input-group label {
            font-size: 14px;
            font-weight: 500;
            color: #4b5563;
        }

        .input-group input,
        .input-group textarea {
            width: 100%;
            padding: 8px 12px;
            border: 1px solid #d1d5db;
            border-radius: 4px;
            font-size: 14px;
            transition: border-color 0.15s ease-in-out;
        }

        .input-group input:focus,
        .input-group textarea:focus {
            outline: none;
            border-color: #10b981;
            box-shadow: 0 0 0 2px rgba(16, 185, 129, 0.1);
        }

        .input-group textarea {
            resize: vertical;
            min-height: 80px;
        }

        .send-button {
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
            background-color: #10b981;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-size: 14px;
            font-weight: 500;
            cursor: pointer;
            transition: background-color 0.15s ease-in-out;
        }

        .send-button:hover {
            background-color: #059669;
        }

        .send-button:active {
            background-color: #047857;
        }

        .send-icon {
            height: 16px;
            width: 16px;
            stroke-width: 2;
        }

        .auto-control {
            margin-top: 8px;
            padding: 8px;
            background-color: #f9fafb;
            border-radius: 4px;
            width: 100%;
        }

        .auto-control-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 4px;
        }

        .auto-control-title {
            font-size: 14px;
            font-weight: 500;
        }

        @keyframes pulse {
            0%,
            100% {
                opacity: 1;
            }
            50% {
                opacity: 0.5;
            }
        }

        .system-description {
            background-color: white;
            border-radius: 8px;
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
            padding: 24px;
            margin-bottom: 32px;
        }

        .description-title {
            font-size: 22px;
            font-weight: 600;
            color: #16a34a;
            margin-bottom: 16px;
            border-bottom: 2px solid #e5e7eb;
            padding-bottom: 8px;
        }

        .description-content {
            font-size: 16px;
            line-height: 1.6;
            color: #4b5563;
        }

        .description-content p {
            margin-bottom: 16px;
        }

        .description-content p:last-child {
            margin-bottom: 0;
        }

        .team-section {
            background-color: white;
            border-radius: 8px;
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
            padding: 24px;
            margin-top: 32px;
        }

        .team-title {
            font-size: 22px;
            font-weight: 600;
            color: #16a34a;
            margin-bottom: 16px;
            border-bottom: 2px solid #e5e7eb;
            padding-bottom: 8px;
        }

        .team-members {
            display: grid;
            grid-template-columns: repeat(1, 1fr);
            gap: 24px;
        }

        @media (min-width: 768px) {
            .team-members {
                grid-template-columns: repeat(2, 1fr);
            }
        }

        @media (min-width: 1024px) {
            .team-members {
                grid-template-columns: repeat(3, 1fr);
            }
        }

        .team-member {
            display: flex;
            flex-direction: column;
            align-items: center;
            text-align: center;
        }

        .member-avatar {
            width: 100px;
            height: 100px;
            border-radius: 50%;
            background-color: #e5e7eb;
            display: flex;
            align-items: center;
            justify-content: center;
            margin-bottom: 12px;
            color: #16a34a;
            font-size: 36px;
            font-weight: bold;
        }

        .member-name {
            font-size: 18px;
            font-weight: 600;
            margin-bottom: 4px;
        }

        .member-role {
            font-size: 14px;
            color: #6b7280;
        }

        .member-description {
            font-size: 14px;
            color: #4b5563;
            margin-top: 8px;
            line-height: 1.4;
        }

        /* New styles for the system model and components */
        .model-section {
            background-color: white;
            border-radius: 8px;
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
            padding: 24px;
            margin-bottom: 32px;
        }

        .model-diagram {
            background-color: #f8fafc;
            border-radius: 8px;
            padding: 24px;
            margin: 20px 0;
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        .system-model {
            max-width: 100%;
            height: auto;
            margin-bottom: 20px;
        }

        .model-description {
            font-size: 16px;
            line-height: 1.6;
            color: #4b5563;
            text-align: center;
        }

        .components-grid {
            display: grid;
            grid-template-columns: repeat(1, 1fr);
            gap: 20px;
            margin-top: 20px;
        }

        @media (min-width: 768px) {
            .components-grid {
                grid-template-columns: repeat(2, 1fr);
            }
        }

        @media (min-width: 1024px) {
            .components-grid {
                grid-template-columns: repeat(3, 1fr);
            }
        }

        .component-card {
            background-color: #f8fafc;
            border-radius: 8px;
            padding: 16px;
            display: flex;
            flex-direction: column;
            align-items: center;
            text-align: center;
        }

        .component-icon {
            width: 60px;
            height: 60px;
            margin-bottom: 12px;
            color: #16a34a;
        }

        .component-title {
            font-size: 18px;
            font-weight: 600;
            margin-bottom: 8px;
            color: #16a34a;
        }

        .component-desc {
            font-size: 14px;
            color: #4b5563;
            line-height: 1.5;
        }

        /* Charts section styles */
        .charts-section {
            background-color: white;
            border-radius: 8px;
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
            padding: 24px;
            margin-bottom: 32px;
        }

        .charts-grid {
            display: grid;
            grid-template-columns: repeat(1, 1fr);
            gap: 24px;
        }

        @media (min-width: 768px) {
            .charts-grid {
                grid-template-columns: repeat(2, 1fr);
            }
        }

        .chart-container {
            position: relative;
            height: 300px;
            width: 100%;
        }

        .chart-title {
            font-size: 18px;
            font-weight: 600;
            margin-bottom: 16px;
            color: #16a34a;
            text-align: center;
        }

        /* Process section */
        .process-section {
            background-color: white;
            border-radius: 8px;
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
            padding: 24px;
            margin-bottom: 32px;
        }

        .process-steps {
            margin-top: 24px;
        }

        .process-step {
            display: flex;
            margin-bottom: 24px;
            position: relative;
        }

        .step-number {
            width: 40px;
            height: 40px;
            border-radius: 50%;
            background-color: #16a34a;
            color: white;
            display: flex;
            align-items: center;
            justify-content: center;
            font-weight: bold;
            margin-right: 16px;
            flex-shrink: 0;
        }

        .step-content {
            padding-bottom: 24px;
            border-left: 2px dashed #16a34a;
            padding-left: 16px;
            margin-left: -2px;
        }

        .step-title {
            font-size: 18px;
            font-weight: 600;
            margin-bottom: 8px;
            color: #16a34a;
        }

        .step-desc {
            font-size: 14px;
            color: #4b5563;
            line-height: 1.5;
        }

        .process-step:last-child .step-content {
            padding-bottom: 0;
            border-left: none;
        }

        .footer {
            margin-top: 48px;
            border-top: 1px solid #e5e7eb;
            padding-top: 24px;
            text-align: center;
        }

        .footer-content {
            display: flex;
            flex-direction: column;
            gap: 8px;
        }

        .footer-title {
            font-size: 16px;
            font-weight: 600;
            color: #4b5563;
        }

        .footer-text {
            font-size: 14px;
            color: #6b7280;
        }
    </style>
</head>

<body>
    <div class="container">
        <h1 class="garden-title">Garden Automation and Growth Optimization</h1>
        <h2 class="garden-subtitle">Smart Monitoring & Control System</h2>

        <section id="dashboard" class="dashboard-section">
            <h2 class="section-title">System Dashboard</h2>
            <div class="garden-grid">
                <div class="garden-card">
                    <div class="garden-card-header">
                        <h3 class="garden-card-title">Water Pump And Solenoid ValveControl</h3>
                    </div>
                    <div class="garden-card-content">
                        <svg id="waterPumpIcon" class="status-icon water-icon inactive" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round">
                            <path d="M19 6h-7a2 2 0 0 0-2 2v9a2 2 0 0 0 2 2h7M6 6h.01M13 6h.01M6 10h.01M13 10h.01M6 14h.01M6 18h.01M13 11l1-1M13 18l1-1"></path>
                            <path d="M18 2v17M18 9l3 3-3 3"></path>
                            <path d="M6 2L3 5l3 3M22 19h-7a2 2 0 0 1-2-2V5"></path>
                        </svg>
                        <div class="status-toggle">
                            <span class="status-label">Manual Control</span>
                            <label class="switch">
                                <input type="checkbox" id="waterPumpToggle">
                                <span class="slider water"></span>
                            </label>
                        </div>
                        <div class="auto-control">
                            <div class="auto-control-header">
                                <span class="auto-control-title">Automatic Mode</span>
                                <label class="switch">
                                    <input type="checkbox" id="autoWaterToggle" checked>
                                    <span class="slider"></span>
                                </label>
                            </div>
                            <small>Activates pump when moisture falls below 20%</small>
                        </div>
                    </div>
                </div>
                <div class="garden-card">
                    <div class="garden-card-header">
                        <h3 class="garden-card-title">Grow Light Control</h3>
                    </div>
                    <div class="garden-card-content">
                        <svg id="growLightIcon" class="status-icon light-icon inactive" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round">
                            <circle cx="12" cy="12" r="5"></circle>
                            <line x1="12" y1="1" x2="12" y2="3"></line>
                            <line x1="12" y1="21" x2="12" y2="23"></line>
                            <line x1="4.22" y1="4.22" x2="5.64" y2="5.64"></line>
                            <line x1="18.36" y1="18.36" x2="19.78" y2="19.78"></line>
                            <line x1="1" y1="12" x2="3" y2="12"></line>
                            <line x1="21" y1="12" x2="23" y2="12"></line>
                            <line x1="4.22" y1="19.78" x2="5.64" y2="18.36"></line>
                            <line x1="18.36" y1="5.64" x2="19.78" y2="4.22"></line>
                        </svg>
                        <div class="status-toggle">
                            <span class="status-label">Manual Control</span>
                            <label class="switch">
                                <input type="checkbox" id="growLightToggle">
                                <span class="slider light"></span>
                            </label>
                        </div>
                        <div class="auto-control">
                            <div class="auto-control-header">
                                <span class="auto-control-title">Automatic Mode</span>
                                <label class="switch">
                                    <input type="checkbox" id="autoLightToggle" checked>
                                    <span class="slider"></span>
                                </label>
                            </div>
                            <small>Activates light when brightness falls below 30%</small>
                        </div>
                    </div>
                </div>
                <div class="garden-card">
                    <div class="garden-card-header">
                        <h3 class="garden-card-title">Soil Moisture</h3>
                    </div>
                    <div class="garden-card-content">
                        <div class="moisture-gauge">
                            <div class="moisture-sensor-label">Sensor 1</div>
                            <div class="moisture-labels">
                                <span>Dry</span>
                                <span>Moist</span>
                                <span>Wet</span>
                            </div>
                            <div class="progress-container">
                                <div id="moisture0Bar" class="progress-bar moist" style="width: 45%"></div>
                            </div>
                            <div class="moisture-value">
                                <span id="moisture0Value" class="moisture-percent">45%</span>
                                <span id="moisture0Status" class="moisture-status">Moist</span>
                            </div>

                            <div class="moisture-sensor-divider"></div>

                            <div class="moisture-sensor-label">Sensor 2</div>
                            <div class="moisture-labels">
                                <span>Dry</span>
                                <span>Moist</span>
                                <span>Wet</span>
                            </div>
                            <div class="progress-container">
                                <div id="moisture1Bar" class="progress-bar wet" style="width: 70%"></div>
                            </div>
                            <div class="moisture-value">
                                <span id="moisture1Value" class="moisture-percent">70%</span>
                                <span id="moisture1Status" class="moisture-status">Wet</span>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="garden-card">
                    <div class="garden-card-header">
                        <h3 class="garden-card-title">Light Level</h3>
                    </div>
                    <div class="garden-card-content">
                        <div class="moisture-gauge">
                            <div class="moisture-labels">
                                <span>Dark</span>
                                <span>Dim</span>
                                <span>Bright</span>
                            </div>
                            <div class="progress-container">
                                <div id="ldrBar" class="progress-bar bright" style="width: 80%"></div>
                            </div>
                            <div class="moisture-value">
                                <span id="ldrValue" class="moisture-percent">80%</span>
                                <span id="ldrStatus" class="moisture-status">Bright</span>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
            <div class="garden-grid" style="margin-top: 24px">
                <div class="garden-card">
                    <div class="garden-card-header">
                        <h3 class="garden-card-title">Plant Height</h3>
                    </div>
                    <div class="garden-card-content">
                        <div id="plantNormal" class="plant-normal">
                            <svg class="leaf-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round">
                                <path d="M2 22l10-10"></path>
                                <path d="M16 6l6-6"></path>
                                <path d="M2 10l10 10"></path>
                                <path d="M9.5 4.5L16 11"></path>
                                <path d="M14 2l-7 7"></path>
                                <path d="M22 8l-9.5 9.5"></path>
                            </svg>
                            <div class="status-text">
                                <div>Plant Height: <span id="tofValue">450</span> mm</div>
                                <div>Plant growth normal</div>
                            </div>
                        </div>
                        <div id="plantAlert" class="plant-alert" style="display: none">
                            <div class="plant-alert-header">
                                <svg class="leaf-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round">
                                    <path d="M2 22l10-10"></path>
                                    <path d="M16 6l6-6"></path>
                                    <path d="M2 10l10 10"></path>
                                    <path d="M9.5 4.5L16 11"></path>
                                    <path d="M14 2l-7 7"></path>
                                    <path d="M22 8l-9.5 9.5"></path>
                                </svg>
                                <div>
                                    <div class="plant-alert-title">Plant needs pruning!</div>
                                    <div id="tofAlertValue" class="plant-alert-desc">Current height: 650 mm</div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="garden-card">
                    <div class="garden-card-header">
                        <h3 class="garden-card-title">Send SMS Alert</h3>
                    </div>
                    <div class="garden-card-content">
                        <form id="smsForm" class="sms-form">
                            <div class="input-group">
                                <label for="phoneNumber">Phone Number</label>
                                <input type="tel" id="phoneNumber" name="phoneNumber" placeholder="+639XXXXXXXXX" required>
                            </div>
                            <div class="input-group">
                                <label for="message">Message</label>
                                <textarea id="message" name="message" placeholder="Enter your message here..." required></textarea>
                            </div>
                            <button id="sendSmsBtn" type="submit" class="send-button">
                                <svg class="send-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-linecap="round" stroke-linejoin="round">
                                    <line x1="22" y1="2" x2="11" y2="13"></line>
                                    <polygon points="22 2 15 22 11 13 2 9 22 2"></polygon>
                                </svg>
                                Send Message
                            </button>
                        </form>
                    </div>
                </div>
            </div>
        </section>

        <section id="system-desc" class="system-description">
            <h2 class="description-title">System Description</h2>
            <div class="description-content">
                <p>The Garden Automation and Growth Optimization system is an advanced solution designed to automate and optimize plant growth in home or small-scale gardening environments. The system integrates multiple sensors and actuators controlled by an ESP32 microcontroller to create the ideal growing conditions for plants.</p>
                
                <p>Our system continuously monitors key growing parameters including soil moisture levels using dual moisture sensors, ambient light conditions via a Light Dependent Resistor (LDR), and plant height through a Time-of-Flight (ToF) distance sensor. Based on these measurements, the system can automatically control a water pump to maintain optimal soil moisture and activate grow lights when natural light is insufficient.</p>
                
                <p>The system features both automatic and manual control modes, allowing users to override automated functions when desired. Additionally, SMS notifications via a GSM module provide real-time alerts about plant conditions, especially when plants grow too tall and may need pruning.</p>
                
                <p>With a responsive web interface accessible over WiFi, users can monitor their garden's status, view historical data trends, and control actuators from any device connected to the local network. This makes the Garden Automation and Growth Optimization system an ideal solution for both experienced gardeners seeking convenience and beginners looking to improve their gardening success.</p>
            </div>
        </section>

        <section id="process" class="process-section">
            <h2 class="description-title">How The System Works</h2>
            <div class="process-steps">
                <div class="process-step">
                    <div class="step-number">1</div>
                    <div class="step-content">
                        <h3 class="step-title">Continuous Monitoring</h3>
                        <p class="step-desc">The system continuously collects data from multiple sensors: two soil moisture sensors to measure moisture levels at different locations, an LDR sensor to detect ambient light conditions, and a ToF sensor to monitor plant height. This data is processed by the ESP32 microcontroller and stored in a historical database.</p>
                    </div>
                </div>
                <div class="process-step">
                    <div class="step-number">2</div>
                    <div class="step-content">
                        <h3 class="step-title">Automated Decision Making</h3>
                        <p class="step-desc">Based on predefined thresholds, the system automatically makes decisions to maintain optimal growing conditions. If the average soil moisture level falls below 20%, the water pump is activated. Similarly, if the ambient light level drops below 30%, the grow lights are turned on to ensure plants receive adequate light for photosynthesis.</p>
                    </div>
                </div>
                <div class="process-step">
                    <div class="step-number">3</div>
                    <div class="step-content">
                        <h3 class="step-title">User Interface & Manual Control</h3>
                        <p class="step-desc">The system provides a web-based interface accessible via WiFi, allowing users to monitor all sensor readings in real-time. The interface also offers manual control over the water pump and grow lights, letting users override automatic functions when needed. Users can toggle between automatic and manual modes for each actuator.</p>
                    </div>
                </div>
                <div class="process-step">
                    <div class="step-number">4</div>
                    <div class="step-content">
                        <h3 class="step-title">SMS Notifications</h3>
                        <p class="step-desc">Using a GSM module, the system can send SMS alerts to notify users about important conditions. For example, when plants grow too tall (detected by the ToF sensor), the system sends a message suggesting pruning. Users can also send custom SMS messages directly from the web interface to check on their garden remotely.</p>
                    </div>
                </div>
                <div class="process-step">
                    <div class="step-number">5</div>
                    <div class="step-content">
                        <h3 class="step-title">Data Visualization & Analysis</h3>
                        <p class="step-desc">Historical data from all sensors is displayed in interactive graphs, allowing users to observe trends and patterns over time. This helps in understanding plant growth cycles, watering needs, and light requirements, enabling users to make informed decisions about their gardening practices.</p>
                    </div>
                </div>
            </div>
        </section>

        <section id="system-model" class="model-section">
            <h2 class="description-title">System Model</h2>
            <div class="model-diagram">
                <svg class="system-model" width="800" height="500" viewBox="0 0 800 500">
                    <!-- ESP32 Microcontroller -->
                    <rect x="340" y="220" width="120" height="60" rx="5" fill="#16a34a" />
                    <text x="400" y="255" text-anchor="middle" fill="white" font-weight="bold">ESP32</text>
                    <text x="400" y="275" text-anchor="middle" fill="white" font-size="12">Microcontroller</text>

                    <!-- Sensors -->
                    <rect x="150" y="120" width="100" height="50" rx="5" fill="#0ea5e9" />
                    <text x="200" y="145" text-anchor="middle" fill="white" font-weight="bold">Moisture</text>
                    <text x="200" y="160" text-anchor="middle" fill="white" font-size="12">Sensors</text>
                    
                    <rect x="150" y="220" width="100" height="50" rx="5" fill="#0ea5e9" />
                    <text x="200" y="245" text-anchor="middle" fill="white" font-weight="bold">Light (LDR)</text>
                    <text x="200" y="260" text-anchor="middle" fill="white" font-size="12">Sensor</text>
                    
                    <rect x="150" y="320" width="100" height="50" rx="5" fill="#0ea5e9" />
                    <text x="200" y="345" text-anchor="middle" fill="white" font-weight="bold">ToF Height</text>
                    <text x="200" y="360" text-anchor="middle" fill="white" font-size="12">Sensor</text>

                    <!-- Actuators -->
                    <rect x="550" y="170" width="100" height="50" rx="5" fill="#d97706" />
                    <text x="600" y="195" text-anchor="middle" fill="white" font-weight="bold">Grow Light</text>
                    <text x="600" y="210" text-anchor="middle" fill="white" font-size="12">Actuator</text>
                    
                    <rect x="550" y="270" width="100" height="50" rx="5" fill="#0369a1" />
                    <text x="600" y="295" text-anchor="middle" fill="white" font-weight="bold">Water Pump</text>
                    <text x="600" y="310" text-anchor="middle" fill="white" font-size="12">Actuator</text>

                    <!-- Communication -->
                    <rect x="340" y="100" width="120" height="50" rx="5" fill="#7c3aed" />
                    <text x="400" y="125" text-anchor="middle" fill="white" font-weight="bold">WiFi</text>
                    <text x="400" y="140" text-anchor="middle" fill="white" font-size="12">Web Interface</text>
                    
                    <rect x="340" y="350" width="120" height="50" rx="5" fill="#7c3aed" />
                    <text x="400" y="375" text-anchor="middle" fill="white" font-weight="bold">GSM Module</text>
                    <text x="400" y="390" text-anchor="middle" fill="white" font-size="12">SMS Notifications</text>

                    <!-- Connection lines -->
                    <!-- Sensors to ESP32 -->
                    <line x1="250" y1="145" x2="340" y2="230" stroke="#9ca3af" stroke-width="2" stroke-dasharray="5,5" />
                    <line x1="250" y1="245" x2="340" y2="250" stroke="#9ca3af" stroke-width="2" stroke-dasharray="5,5" />
                    <line x1="250" y1="345" x2="340" y2="270" stroke="#9ca3af" stroke-width="2" stroke-dasharray="5,5" />
                    
                    <!-- ESP32 to Actuators -->
                    <line x1="460" y1="230" x2="550" y2="195" stroke="#9ca3af" stroke-width="2" stroke-dasharray="5,5" />
                    <line x1="460" y1="270" x2="550" y2="295" stroke="#9ca3af" stroke-width="2" stroke-dasharray="5,5" />
                    
                    <!-- ESP32 to Communication -->
                    <line x1="400" y1="220" x2="400" y2="150" stroke="#9ca3af" stroke-width="2" stroke-dasharray="5,5" />
                    <line x1="400" y1="280" x2="400" y2="350" stroke="#9ca3af" stroke-width="2" stroke-dasharray="5,5" />
                </svg>
                <p class="model-description">System architecture showing the interconnection between sensors, actuators, and communication modules through the central ESP32 microcontroller.</p>
            </div>
        </section>

        <section id="components" class="model-section">
            <h2 class="description-title">System Components</h2>
            <div class="components-grid">
                <div class="component-card">
                    <svg class="component-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">
                        <rect x="2" y="4" width="20" height="16" rx="2" />
                        <path d="M8 2v4"></path>
                        <path d="M16 2v4"></path>
                        <path d="M4 9h16"></path>
                        <path d="M6 12h12"></path>
                        <path d="M6 15h12"></path>
                        <path d="M8 18h8"></path>
                    </svg>
                    <h3 class="component-title">ESP32 Microcontroller</h3>
                    <p class="component-desc">The brain of the system, ESP32 processes sensor data, controls actuators, and provides WiFi connectivity for the web interface. It runs the automation logic and handles all communication between components.</p>
                </div>
                
                <div class="component-card">
                    <svg class="component-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M4 22h14"></path>
                        <path d="M14 22a2 2 0 0 0 2-2"></path>
                        <path d="M16 7a2 2 0 0 0-2-2"></path>
                        <path d="M14 5H4"></path>
                        <path d="M4 2v20"></path>
                        <path d="M22 14v1a5 5 0 0 1-5 5"></path>
                        <path d="M22 7.5v2.83a2.17 2.17 0 0 1-.83 1.67l-2.67 2"></path>
                        <path d="M18 2A2 2 0 0 0 16 3"></path>
                        <path d="M18 15h-4"></path>
                        <path d="M18 11h-5"></path>
                        <rect x="4" y="8" width="8" height="3" rx="1" />
                    </svg>
                    <h3 class="component-title">Soil Moisture Sensors</h3>
                    <p class="component-desc">Two resistive soil moisture sensors measure water content in the soil at different locations. They provide analog readings that are converted to percentages to determine when watering is needed.</p>
                </div>
                
                <div class="component-card">
                    <svg class="component-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">
                        <circle cx="12" cy="12" r="5"></circle>
                        <line x1="12" y1="1" x2="12" y2="3"></line>
                        <line x1="12" y1="21" x2="12" y2="23"></line>
                        <line x1="4.22" y1="4.22" x2="5.64" y2="5.64"></line>
                        <line x1="18.36" y1="18.36" x2="19.78" y2="19.78"></line>
                        <line x1="1" y1="12" x2="3" y2="12"></line>
                        <line x1="21" y1="12" x2="23" y2="12"></line>
                        <line x1="4.22" y1="19.78" x2="5.64" y2="18.36"></line>
                        <line x1="18.36" y1="5.64" x2="19.78" y2="4.22"></line>
                    </svg>
                    <h3 class="component-title">Light Dependent Resistor</h3>
                    <p class="component-desc">The LDR sensor detects ambient light levels, providing data to determine when supplemental lighting is required. It helps ensure plants receive adequate light for optimal photosynthesis.</p>
                </div>
                
                <div class="component-card">
                    <svg class="component-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M2 16h20M2 8h20M9 16c0 4 3 5 3 5s3-1 3-5"></path>
                        <path d="M12 16V3"></path>
                    </svg>
                    <h3 class="component-title">ToF Distance Sensor</h3>
                    <p class="component-desc">The Time-of-Flight (VL53L0X) sensor accurately measures plant height by calculating the distance between the sensor and the plant top. It allows for monitoring plant growth and triggers alerts when pruning is needed.</p>
                </div>
                
                <div class="component-card">
                    <svg class="component-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M8 5H4a1 1 0 0 0-1 1v10a1 1 0 0 0 1 1h4"></path>
                        <path d="M16 15V9a1 1 0 0 0-1-1h-3a1 1 0 0 0-1 1v6"></path>
                        <rect x="4" y="15" width="4" height="4" rx="1" ry="1"></rect>
                        <rect x="11" y="15" width="4" height="4" rx="1" ry="1"></rect>
                        <path d="M16 8h4a1 1 0 0 1 1 1v5a1 1 0 0 1-1 1h-4"></path>
                        <circle cx="6" cy="9" r="1"></circle>
                    </svg>
                    <h3 class="component-title">Water Pump and Solenoid Valve System</h3>
                    <p class="component-desc">A 5-12V water pump connected through a relay module delivers water to plants when soil moisture is low. The system includes a 12V solenoid valve to control water flow direction and prevent backflow.</p>
                </div>
                
                <div class="component-card">
                    <svg class="component-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"></path>
                    </svg>
                    <h3 class="component-title">Grow Light</h3>
                    <p class="component-desc">LED grow lights provide supplemental lighting for plants when natural light is insufficient. They're automatically activated in low-light conditions and can be manually controlled through the web interface.</p>
                </div>
                
                <div class="component-card">
                    <svg class="component-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M2 3h6a4 4 0 0 1 4 4v14a3 3 0 0 0-3-3H2z"></path>
                        <path d="M22 3h-6a4 4 0 0 0-4 4v14a3 3 0 0 1 3-3h7z"></path>
                    </svg>
                    <h3 class="component-title">Relay Module</h3>
                    <p class="component-desc">A multi-channel relay module controls high-power devices (water pump and grow lights) from the low-power ESP32 microcontroller signals, providing electrical isolation and safe switching capabilities.</p>
                </div>
                
                <div class="component-card">
                    <svg class="component-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M17 2h3a2 2 0 0 1 2 2v16a2 2 0 0 1-2 2h-3"></path>
                        <path d="M7 2H4a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h3"></path>
                        <path d="M7 12h10"></path>
                        <path d="M7 5v14"></path>
                        <path d="M17 5v14"></path>
                    </svg>
                    <h3 class="component-title">GSM Module</h3>
                    <p class="component-desc">The SIM800L GSM module enables SMS communication, allowing the system to send alerts and status updates to the user's mobile phone. It connects to the ESP32 via software serial interface.</p>
                </div>
                
                <div class="component-card">
                    <svg class="component-icon" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linecap="round" stroke-linejoin="round">
                        <path d="M5 12.55a11 11 0 0 1 14.08 0"></path>
                        <path d="M1.42 9a16 16 0 0 1 21.16 0"></path>
                        <path d="M8.53 16.11a6 6 0 0 1 6.95 0"></path>
                        <line x1="12" y1="20" x2="12.01" y2="20"></line>
                    </svg>
                    <h3 class="component-title">WiFi Connection</h3>
                    <p class="component-desc">The ESP32's integrated WiFi capability creates a local access point, allowing users to connect to the web interface from any device on the network without requiring an external router or internet connection.</p>
                </div>
            </div>
        </section>

        <section id="sensor-data" class="charts-section">
            <h2 class="description-title">Sensor Data History</h2>
            <div class="charts-grid">
                <div>
                    <h3 class="chart-title">Soil Moisture History</h3>
                    <div class="chart-container">
                        <canvas id="moistureChart"></canvas>
                    </div>
                </div>
                <div>
                    <h3 class="chart-title">Light Level History</h3>
                    <div class="chart-container">
                        <canvas id="lightChart"></canvas>
                    </div>
                </div>
                <div>
                    <h3 class="chart-title">Plant Height History</h3>
                    <div class="chart-container">
                        <canvas id="heightChart"></canvas>
                    </div>
                </div>
            </div>
        </section>

        <section id="team" class="team-section">
            <h2 class="team-title">Development Team BSECE-3A 2425</h2>
            <div class="team-members">
                <div class="team-member">
                    <div class="member-avatar">JC</div>
                    <h3 class="member-name">Juliana Cardoso</h3>
                    <p class="member-role">Presentation & Hardware Support</p>
                    <p class="member-description">Juliana designed the PowerPoint presentation, contributed to the construction of the main prototype, assisted in sensor testing, and helped source the necessary materials.</p>
                </div>
                <div class="team-member">
                    <div class="member-avatar">JCM</div>
                    <h3 class="member-name">Jim Clark Mengol</h3>
                    <p class="member-role">Embedded Systems & Troubleshooting</p>
                    <p class="member-description">Jim worked on programming the sensors using ESP32, participated in building the main prototype, and played a key role in testing and troubleshooting both the sensors and the program.</p>
                </div>
                <div class="team-member">
                    <div class="member-avatar">RR</div>
                    <h3 class="member-name">Ricarena Recaña</h3>
                    <p class="member-role">Web Development & Software Integration</p>
                    <p class="member-description">Ricarena developed the project website, programmed the sensors using ESP32, and assisted in gathering the required materials.</p>
                </div>
            </div>
        </section>

        <!-- Footer -->
        <footer class="footer">
            <div class="footer-content">
                <h3 class="footer-title">Garden Automation and Growth Optimization</h3>
                <p class="footer-text">BSECE-3A-G5 © 2023 | A smart solution for optimal plant growth</p>
            </div>
        </footer>
    </div>

    <script>
        // DOM elements
        const waterPumpToggle = document.getElementById('waterPumpToggle');
        const growLightToggle = document.getElementById('growLightToggle');
        const autoWaterToggle = document.getElementById('autoWaterToggle');
        const autoLightToggle = document.getElementById('autoLightToggle');
        const waterPumpIcon = document.getElementById('waterPumpIcon');
        const growLightIcon = document.getElementById('growLightIcon');
        const moisture0Bar = document.getElementById('moisture0Bar');
        const moisture0Value = document.getElementById('moisture0Value');
        const moisture0Status = document.getElementById('moisture0Status');
        const moisture1Bar = document.getElementById('moisture1Bar');
        const moisture1Value = document.getElementById('moisture1Value');
        const moisture1Status = document.getElementById('moisture1Status');
        const ldrBar = document.getElementById('ldrBar');
        const ldrValue = document.getElementById('ldrValue');
        const ldrStatus = document.getElementById('ldrStatus');
        const tofValue = document.getElementById('tofValue');
        const plantNormal = document.getElementById('plantNormal');
        const plantAlert = document.getElementById('plantAlert');
        const tofAlertValue = document.getElementById('tofAlertValue');
        const smsForm = document.getElementById('smsForm');
        const sendSmsBtn = document.getElementById('sendSmsBtn');

        // Initial states
        let waterPumpState = 0;
        let growLightState = 0;
        let autoWaterState = 1;
        let autoLightState = 1;
        let moisture0 = 45;
        let moisture1 = 70;
        let ldr = 80;
        let tof = 450;

        // Charts
        let moistureChart, lightChart, heightChart;
        let moistureData = Array(20).fill().map(() => Math.floor(Math.random() * 50) + 30);
        let moisture2Data = Array(20).fill().map(() => Math.floor(Math.random() * 40) + 50);
        let lightData = Array(20).fill().map(() => Math.floor(Math.random() * 30) + 60);
        let heightData = Array(20).fill().map((_, i) => 300 + i * 7);

        // Toggle handlers
        waterPumpToggle.addEventListener('change', function() {
            waterPumpState = this.checked ? 1 : 0;
            updateWaterPumpDisplay();
            setPinState('waterPump', this.checked ? 'ON' : 'OFF');
        });

        growLightToggle.addEventListener('change', function() {
            growLightState = this.checked ? 1 : 0;
            updateGrowLightDisplay();
            setPinState('growLight', this.checked ? 'ON' : 'OFF');
        });

        autoWaterToggle.addEventListener('change', function() {
            autoWaterState = this.checked ? 1 : 0;
            setPinState('autoWater', this.checked ? 'ON' : 'OFF');
        });

        autoLightToggle.addEventListener('change', function() {
            autoLightState = this.checked ? 1 : 0;
            setPinState('autoLight', this.checked ? 'ON' : 'OFF');
        });

        // SMS form handler
        smsForm.addEventListener('submit', function(e) {
            e.preventDefault();
            const phoneNumber = document.getElementById('phoneNumber').value;
            const message = document.getElementById('message').value;
            
            sendSmsMessage(phoneNumber, message);
            
            // Reset form after sending
            document.getElementById('message').value = '';
        });

        // Update display functions
        function updateWaterPumpDisplay() {
            if (waterPumpState) {
                waterPumpIcon.classList.remove('inactive');
                waterPumpIcon.classList.add('active');
            } else {
                waterPumpIcon.classList.remove('active');
                waterPumpIcon.classList.add('inactive');
            }
            waterPumpToggle.checked = waterPumpState;
        }

        function updateGrowLightDisplay() {
            if (growLightState) {
                growLightIcon.classList.remove('inactive');
                growLightIcon.classList.add('active');
            } else {
                growLightIcon.classList.remove('active');
                growLightIcon.classList.add('inactive');
            }
            growLightToggle.checked = growLightState;
        }

        function updateMoistureSensor0Display() {
            moisture0Bar.style.width = `${moisture0}%`;
            moisture0Value.textContent = `${moisture0}%`;

            moisture0Bar.classList.remove('dry', 'moist', 'wet');
            if (moisture0 < 30) {
                moisture0Bar.classList.add('dry');
                moisture0Status.textContent = 'Dry';
            } else if (moisture0 < 70) {
                moisture0Bar.classList.add('moist');
                moisture0Status.textContent = 'Moist';
            } else {
                moisture0Bar.classList.add('wet');
                moisture0Status.textContent = 'Wet';
            }
        }

        function updateMoistureSensor1Display() {
            moisture1Bar.style.width = `${moisture1}%`;
            moisture1Value.textContent = `${moisture1}%`;

            moisture1Bar.classList.remove('dry', 'moist', 'wet');
            if (moisture1 < 30) {
                moisture1Bar.classList.add('dry');
                moisture1Status.textContent = 'Dry';
            } else if (moisture1 < 70) {
                moisture1Bar.classList.add('moist');
                moisture1Status.textContent = 'Moist';
            } else {
                moisture1Bar.classList.add('wet');
                moisture1Status.textContent = 'Wet';
            }
        }

        function updateLdrDisplay() {
            ldrBar.style.width = `${ldr}%`;
            ldrValue.textContent = `${ldr}%`;

            ldrBar.classList.remove('dark', 'dim', 'bright');
            if (ldr < 30) {
                ldrBar.classList.add('dark');
                ldrStatus.textContent = 'Dark';
            } else if (ldr < 70) {
                ldrBar.classList.add('dim');
                ldrStatus.textContent = 'Dim';
            } else {
                ldrBar.classList.add('bright');
                ldrStatus.textContent = 'Bright';
            }
        }

        function updateTofDisplay() {
            tofValue.textContent = tof;
            tofAlertValue.textContent = `Current height: ${tof} mm`;

            if (tof > 600) {
                plantNormal.style.display = 'none';
                plantAlert.style.display = 'block';
            } else {
                plantNormal.style.display = 'flex';
                plantAlert.style.display = 'none';
            }
        }

        // API communication functions
        function setPinState(pinName, state) {
            fetch(`/setPinState?name=${pinName}&state=${state}`)
                .then(response => response.text())
                .catch(error => console.error('Error setting pin state:', error));
        }

        function sendSmsMessage(phoneNumber, message) {
            fetch(`/sendSmsMessage?phoneNumber=${encodeURIComponent(phoneNumber)}&message=${encodeURIComponent(message)}`)
                .then(response => response.text())
                .then(() => {
                    alert('Message sent successfully!');
                })
                .catch(error => console.error('Error sending SMS:', error));
        }

        function fetchSystemStates() {
            fetch('/getStates')
                .then(response => response.json())
                .then(data => {
                    // Update state variables
                    waterPumpState = data.waterPump;
                    growLightState = data.growLight;
                    autoWaterState = data.autoWater;
                    autoLightState = data.autoLight;
                    moisture0 = data.moisture0Value;
                    moisture1 = data.moisture1Value;
                    ldr = data.ldrValue;
                    tof = data.tofValue;

                    // Update UI
                    updateWaterPumpDisplay();
                    updateGrowLightDisplay();
                    autoWaterToggle.checked = autoWaterState;
                    autoLightToggle.checked = autoLightState;
                    updateMoistureSensor0Display();
                    updateMoistureSensor1Display();
                    updateLdrDisplay();
                    updateTofDisplay();
                })
                .catch(error => console.error('Error fetching system states:', error));
        }

        function fetchHistoryData() {
            fetch('/getHistory')
                .then(response => response.json())
                .then(data => {
                    // Update charts with history data
                    moistureChart.data.datasets[0].data = data.moisture0;
                    moistureChart.data.datasets[1].data = data.moisture1;
                    moistureChart.update();

                    lightChart.data.datasets[0].data = data.light;
                    lightChart.update();

                    heightChart.data.datasets[0].data = data.height;
                    heightChart.update();
                })
                .catch(error => console.error('Error fetching history data:', error));
        }

        // Initialize charts
        document.addEventListener('DOMContentLoaded', function() {
            // Moisture Chart
            const moistureCtx = document.getElementById('moistureChart').getContext('2d');
            moistureChart = new Chart(moistureCtx, {
                type: 'line',
                data: {
                    labels: Array(20).fill().map((_, i) => i + 1),
                    datasets: [
                        {
                            label: 'Sensor 1',
                            data: moistureData,
                            borderColor: '#8b5cf6',
                            backgroundColor: 'rgba(139, 92, 246, 0.1)',
                            tension: 0.4,
                            fill: true
                        },
                        {
                            label: 'Sensor 2',
                            data: moisture2Data,
                            borderColor: '#0ea5e9',
                            backgroundColor: 'rgba(14, 165, 233, 0.1)',
                            tension: 0.4,
                            fill: true
                        }
                    ]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    scales: {
                        y: {
                            beginAtZero: true,
                            max: 100,
                            title: {
                                display: true,
                                text: 'Moisture (%)'
                            }
                        },
                        x: {
                            title: {
                                display: true,
                                text: 'Time'
                            }
                        }
                    }
                }
            });

            // Light Chart
            const lightCtx = document.getElementById('lightChart').getContext('2d');
            lightChart = new Chart(lightCtx, {
                type: 'line',
                data: {
                    labels: Array(20).fill().map((_, i) => i + 1),
                    datasets: [
                        {
                            label: 'Light Level',
                            data: lightData,
                            borderColor: '#fbbf24',
                            backgroundColor: 'rgba(251, 191, 36, 0.1)',
                            tension: 0.4,
                            fill: true
                        }
                    ]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    scales: {
                        y: {
                            beginAtZero: true,
                            max: 100,
                            title: {
                                display: true,
                                text: 'Light Level (%)'
                            }
                        },
                        x: {
                            title: {
                                display: true,
                                text: 'Time'
                            }
                        }
                    }
                }
            });

            // Height Chart
            const heightCtx = document.getElementById('heightChart').getContext('2d');
            heightChart = new Chart(heightCtx, {
                type: 'line',
                data: {
                    labels: Array(20).fill().map((_, i) => i + 1),
                    datasets: [
                        {
                            label: 'Plant Height',
                            data: heightData,
                            borderColor: '#4ade80',
                            backgroundColor: 'rgba(74, 222, 128, 0.1)',
                            tension: 0.4,
                            fill: true
                        }
                    ]
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    scales: {
                        y: {
                            beginAtZero: false,
                            title: {
                                display: true,
                                text: 'Height (mm)'
                            }
                        },
                        x: {
                            title: {
                                display: true,
                                text: 'Time'
                            }
                        }
                    }
                }
            });

            // Initial UI updates
            updateMoistureSensor0Display();
            updateMoistureSensor1Display();
            updateLdrDisplay();
            updateTofDisplay();
            updateWaterPumpDisplay();
            updateGrowLightDisplay();

            // Start polling for updates
            setInterval(fetchSystemStates, 2000);
            setInterval(fetchHistoryData, 5000);
            
            // Initial data fetch
            fetchSystemStates();
            fetchHistoryData();
        });
    </script>
</body>

</html>
)=====";