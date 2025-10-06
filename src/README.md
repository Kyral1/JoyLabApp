> **Note**: Make sure you have completed the [Set Up Your Environment](https://reactnative.dev/docs/set-up-your-environment) guide before proceeding.

# Ensure node.js is installed properly
Can verify using the following command: 
```sh
node -v

npm -v
```

# Ensure in correct directory within terminal

# Running the app on an android emulator

## 1. Start metro service
```sh
npm start
```

## 2. Start the emulator
```sh
emulator -list-avds

emulator -avd <device name>
```

## 3. Start Metro Bundler
```sh
npx react-native start
```

## 4. Build and install the application
In a second terminal:

```sh
npx react-native run-android
```