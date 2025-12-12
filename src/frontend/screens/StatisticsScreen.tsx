// src/frontend/screens/StatisticsScreen.tsx
import React, { useEffect, useMemo, useState } from "react";
import {
  View,
  Text,
  StyleSheet,
  ScrollView,
  ActivityIndicator,
  TouchableOpacity,
  FlatList,
} from "react-native";
import { supabase } from "../../backend/app/services/supabase";

type WhackStat = {
  id: string;
  hits: number;
  attempts: number;
  mode: string | null;
  created_at: string;
};

type RegLEDStat = {
  id: string;
  hits: number;
  mode: string | null;
  created_at: string;
};

type SoundStat = {
  id: string;
  hits: number;
  attempts: number;
  mode: string | null;
  created_at: string;
};

type DateFilter = "all" | "7" | "30";

export default function StatisticsScreen() {
  const [whackStats, setWhackStats] = useState<WhackStat[]>([]);
  const [regLEDStats, setRegLEDStats] = useState<RegLEDStat[]>([]);
  const [soundStats, setSoundStats] = useState<SoundStat[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [dateFilter, setDateFilter] = useState<DateFilter>("all");

  console.log("📊 StatisticsScreen loaded!");

  useEffect(() => {
    const fetchAll = async () => {
      setLoading(true);
      setError(null);

      try {
        const [
          { data: whack, error: whackError },
          { data: led, error: ledError },
          { data: sound, error: soundError },
        ] = await Promise.all([
          supabase
            .from("stats")
            .select("*")
            .order("created_at", { ascending: false }),
          supabase
            .from("regLED_stats")
            .select("*")
            .order("created_at", { ascending: false }),
          supabase
            .from("sound_stats")
            .select("*")
            .order("created_at", { ascending: false }),
        ]);

        if (whackError || ledError || soundError) {
          throw (
            whackError ??
            ledError ??
            soundError ??
            new Error("Unknown stats error")
          );
        }

        // 🔁 SWAP hits and attempts for Whack (stats) and Sound
        const fixedWhack = (whack || []).map((row: any) => ({
          ...row,
          hits: row.attempts,    // what DB stored as attempts becomes hits
          attempts: row.hits,    // what DB stored as hits becomes attempts
        })) as WhackStat[];

        const fixedSound = (sound || []).map((row: any) => ({
          ...row,
          hits: row.attempts,
          attempts: row.hits,
        })) as SoundStat[];

        setWhackStats(fixedWhack);
        setRegLEDStats((led || []) as RegLEDStat[]);
        setSoundStats(fixedSound);
      } catch (e: any) {
        console.error("Error loading stats:", e);
        setError(e.message ?? "Failed to load statistics");
      } finally {
        setLoading(false);
      }
    };

    fetchAll();
  }, []);

  // ───────────────────────────── Date Filter Logic ─────────────────────────────

  const minDate: Date | null = useMemo(() => {
    if (dateFilter === "all") return null;

    const now = new Date();
    const days = dateFilter === "7" ? 7 : dateFilter === "30" ? 30 : 0;
    const d = new Date(now);
    d.setDate(now.getDate() - days);
    return d;
  }, [dateFilter]);

  function applyDateFilter<T extends { created_at: string }>(stats: T[]): T[] {
    if (!minDate) return stats;

    return stats.filter((s) => {
      const created = new Date(s.created_at);
      return created >= minDate;
    });
  }

  const filteredWhack = useMemo(
    () => applyDateFilter(whackStats),
    [whackStats, minDate]
  );
  const filteredRegLED = useMemo(
    () => applyDateFilter(regLEDStats),
    [regLEDStats, minDate]
  );
  const filteredSound = useMemo(
    () => applyDateFilter(soundStats),
    [soundStats, minDate]
  );

  // ───────────────────────────── Summaries ─────────────────────────────

  const whackSummary = useMemo(() => {
    const sessions = filteredWhack.length;
    let hits = 0;
    let attempts = 0;
    filteredWhack.forEach((s) => {
      hits += s.hits ?? 0;
      attempts += s.attempts ?? 0;
    });
    const accuracy = attempts > 0 ? hits / attempts : 0;
    return { sessions, hits, attempts, accuracy };
  }, [filteredWhack]);

  const regLEDSummary = useMemo(() => {
    const sessions = filteredRegLED.length;
    let hits = 0;
    filteredRegLED.forEach((s) => {
      hits += s.hits ?? 0;
    });
    return { sessions, hits };
  }, [filteredRegLED]);

  const soundSummary = useMemo(() => {
    const sessions = filteredSound.length;
    let hits = 0;
    let attempts = 0;
    filteredSound.forEach((s) => {
      hits += s.hits ?? 0;
      attempts += s.attempts ?? 0;
    });
    const accuracy = attempts > 0 ? hits / attempts : 0;
    return { sessions, hits, attempts, accuracy };
  }, [filteredSound]);

  // ───────────────────────────── Rendering ─────────────────────────────

  if (loading) {
    return (
      <View style={styles.centered}>
        <ActivityIndicator size="large" color="#4A7FFB" />
      </View>
    );
  }

  if (error) {
    return (
      <View style={styles.centered}>
        <Text style={styles.errorText}>Error: {error}</Text>
      </View>
    );
  }

  return (
    <ScrollView contentContainerStyle={styles.scrollContainer}>
      {/* Header */}
      <Text style={styles.title}>Your JoyLab Stats</Text>
      <Text style={styles.subTitle}>
        Compare your Whack-A-Mole, LED Regular, and Sound Mode games.
      </Text>

      {/* Date Filter Pills */}
      <View style={styles.filterRow}>
        <FilterChip
          label="All time"
          active={dateFilter === "all"}
          onPress={() => setDateFilter("all")}
        />
        <FilterChip
          label="Last 7 days"
          active={dateFilter === "7"}
          onPress={() => setDateFilter("7")}
        />
        <FilterChip
          label="Last 30 days"
          active={dateFilter === "30"}
          onPress={() => setDateFilter("30")}
        />
      </View>

      {/* Whack-A-Mole Section */}
      <StatsSection
        title="Whack-A-Mole"
        summaryLines={[
          `${whackSummary.sessions} session${
            whackSummary.sessions === 1 ? "" : "s"
          }`,
          `Hits: ${whackSummary.hits}`,
          `Attempts: ${whackSummary.attempts}`,
          `Accuracy: ${(whackSummary.accuracy * 100).toFixed(1)}%`,
        ]}
        emptyText="No Whack-A-Mole stats yet."
        data={filteredWhack}
        renderItem={(item) => (
          <SessionRow
            modeLabel="Whack-A-Mole"
            created_at={item.created_at}
            hits={item.hits}
            attempts={item.attempts}
          />
        )}
      />

      {/* LED Regular Section */}
      <StatsSection
        title="LED Regular"
        summaryLines={[
          `${regLEDSummary.sessions} session${
            regLEDSummary.sessions === 1 ? "" : "s"
          }`,
          `Total hits: ${regLEDSummary.hits}`,
        ]}
        emptyText="No LED Regular stats yet."
        data={filteredRegLED}
        renderItem={(item) => (
          <SessionRow
            modeLabel="LED Regular"
            created_at={item.created_at}
            hits={item.hits}
            // no attempts for reg LED
          />
        )}
      />

      {/* Sound Mode Section */}
      <StatsSection
        title="Sound Mode"
        summaryLines={[
          `${soundSummary.sessions} session${
            soundSummary.sessions === 1 ? "" : "s"
          }`,
          `Hits: ${soundSummary.hits}`,
          `Attempts: ${soundSummary.attempts}`,
          `Accuracy: ${(soundSummary.accuracy * 100).toFixed(1)}%`,
        ]}
        emptyText="No Sound Mode stats yet."
        data={filteredSound}
        renderItem={(item) => (
          <SessionRow
            modeLabel="Sound Mode"
            created_at={item.created_at}
            hits={item.hits}
            attempts={item.attempts}
          />
        )}
      />
    </ScrollView>
  );
}

// ───────────────────────────── Small Components ─────────────────────────────

type FilterChipProps = {
  label: string;
  active: boolean;
  onPress: () => void;
};

const FilterChip: React.FC<FilterChipProps> = ({ label, active, onPress }) => (
  <TouchableOpacity
    style={[styles.filterChip, active && styles.filterChipActive]}
    onPress={onPress}
  >
    <Text
      style={[
        styles.filterChipText,
        active && styles.filterChipTextActive,
      ]}
    >
      {label}
    </Text>
  </TouchableOpacity>
);

type StatsSectionProps<T> = {
  title: string;
  summaryLines: string[];
  emptyText: string;
  data: T[];
  renderItem: (item: T) => React.ReactNode;
};

function StatsSection<T>({
  title,
  summaryLines,
  emptyText,
  data,
  renderItem,
}: StatsSectionProps<T>) {
  return (
    <View style={styles.sectionCard}>
      <Text style={styles.sectionTitle}>{title}</Text>

      {/* Summary lines */}
      {summaryLines.map((line, idx) => (
        <Text key={idx} style={styles.summaryLine}>
          {line}
        </Text>
      ))}

      {/* List */}
      {data.length === 0 ? (
        <Text style={styles.emptyText}>{emptyText}</Text>
      ) : (
        <>
          <Text style={styles.recentTitle}>Recent Sessions</Text>
          <FlatList
            data={data}
            keyExtractor={(_, idx) => idx.toString()}
            scrollEnabled={false}
            renderItem={({ item }) => <>{renderItem(item)}</>}
          />
        </>
      )}
    </View>
  );
}

type SessionRowProps = {
  modeLabel: string;
  created_at: string;
  hits: number;
  attempts?: number;
};

const SessionRow: React.FC<SessionRowProps> = ({
  modeLabel,
  created_at,
  hits,
  attempts,
}) => {
  const dateStr = new Date(created_at).toLocaleString();
  const accuracy =
    attempts != null && attempts > 0
      ? ((hits / attempts) * 100).toFixed(1) + "%"
      : undefined;

  return (
    <View style={styles.sessionRow}>
      <View style={{ flex: 1 }}>
        <Text style={styles.sessionMode}>{modeLabel}</Text>
        <Text style={styles.sessionDate}>{dateStr}</Text>
      </View>
      <View style={styles.sessionStats}>
        <Text style={styles.sessionLine}>
          Hits: {hits}
          {attempts != null ? ` / ${attempts}` : ""}
        </Text>
        {accuracy && <Text style={styles.sessionLine}>{accuracy}</Text>}
      </View>
    </View>
  );
};

// ───────────────────────────── Styles ─────────────────────────────

const styles = StyleSheet.create({
  scrollContainer: {
    flexGrow: 1,
    backgroundColor: "#F9FAFF",
    paddingHorizontal: 20,
    paddingTop: 20,
    paddingBottom: 40,
  },
  centered: {
    flex: 1,
    backgroundColor: "#F9FAFF",
    justifyContent: "center",
    alignItems: "center",
  },
  title: {
    fontSize: 26,
    fontWeight: "700",
    color: "#333",
    textAlign: "center",
    marginBottom: 4,
  },
  subTitle: {
    fontSize: 15,
    color: "#555",
    textAlign: "center",
    marginBottom: 18,
  },
  errorText: {
    color: "#E74C3C",
    fontSize: 16,
  },
  filterRow: {
    flexDirection: "row",
    justifyContent: "center",
    marginBottom: 18,
  },
  filterChip: {
    borderRadius: 20,
    borderWidth: 1,
    borderColor: "#C9D6FF",
    paddingVertical: 6,
    paddingHorizontal: 12,
    marginHorizontal: 4,
    backgroundColor: "#F9FAFF",
  },
  filterChipActive: {
    backgroundColor: "#4A7FFB",
    borderColor: "#4A7FFB",
  },
  filterChipText: {
    fontSize: 13,
    color: "#2C3E50",
  },
  filterChipTextActive: {
    color: "#FFFFFF",
    fontWeight: "600",
  },
  sectionCard: {
    backgroundColor: "#FFFFFF",
    borderRadius: 20,
    padding: 18,
    marginBottom: 18,
    shadowColor: "#000",
    shadowOpacity: 0.08,
    shadowRadius: 6,
    elevation: 2,
  },
  sectionTitle: {
    fontSize: 20,
    fontWeight: "600",
    color: "#2C3E50",
    marginBottom: 6,
  },
  summaryLine: {
    fontSize: 14,
    color: "#34495E",
  },
  emptyText: {
    marginTop: 8,
    fontSize: 14,
    color: "#7F8C8D",
    fontStyle: "italic",
  },
  recentTitle: {
    fontSize: 16,
    fontWeight: "600",
    color: "#2C3E50",
    marginTop: 12,
    marginBottom: 4,
  },
  sessionRow: {
    flexDirection: "row",
    alignItems: "center",
    paddingVertical: 8,
    borderBottomWidth: StyleSheet.hairlineWidth,
    borderBottomColor: "#D6DBE9",
  },
  sessionMode: {
    fontSize: 15,
    fontWeight: "600",
    color: "#2C3E50",
  },
  sessionDate: {
    fontSize: 12,
    color: "#7F8C8D",
  },
  sessionStats: {
    alignItems: "flex-end",
    marginLeft: 12,
  },
  sessionLine: {
    fontSize: 14,
    color: "#34495E",
  },
});
