
import 'react-native-url-polyfill/auto'
import AsyncStorage from '@react-native-async-storage/async-storage'
import { createClient, processLock } from '@supabase/supabase-js'
import Config from "react-native-config";

console.log("Supabase URL:", Config.SUPABASE_URL);
console.log("Supabase Key:", Config.SUPABASE_KEY);

export const supabase = createClient(
    Config.SUPABASE_URL!,
    Config.SUPABASE_KEY!,
  {
    auth: {
      storage: AsyncStorage,
      autoRefreshToken: true,
      persistSession: true,
      detectSessionInUrl: false,
      lock: processLock,
    },
  })
        